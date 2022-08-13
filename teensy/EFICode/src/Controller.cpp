#include "Controller.h" //NoiseReduced, Constants.h, Tach.h, spi_adc.h

#include "Arduino.h"
#include "utils/TimerThree.h"
#include "Utils.h"

#include "sensors/ECTSensor.h"
#include "sensors/IATSensor.h"
#include "sensors/TPSSensor.h"
#include "sensors/MAPSensor.h"
#include "sensors/SensorAvg.h"

#include "SD.h"
#include "math.h"

Controller::Controller() {
    // Sets injector pin to output mode. All other pins default to input mode.
    pinMode(INJ_Pin, OUTPUT);

    // Initializes Serial input and output at the specified baud rate.
    Serial.begin(BAUD_RATE);
    Serial1.begin(BAUD_RATE);

    // Prevent blocking caused by the lack of a serial connection with a laptop (fixed W22)
    long t = micros();
    while(!Serial && (micros() - t < 1e6));

    // Initializing message
    Serial.println("Initializing...\n");

    // Initialize parameters with their starting values.
    this->initializeParameters();

    // Update sensors to their initial values.
    this->readSensors();
}


bool Controller::readSensors() {
  if (refreshAvailable){
    m_adc->refresh();
    refreshAvailable = false;
  }

  m_adc->checkEOC();

  if (m_adc->get_validVals() == 1){
    const int* channels = m_adc->getChannels();
    sensorVals = channels;

    m_tps->readSensor(sensorVals);
    m_ect->readSensor(sensorVals);
    m_iat->readSensor(sensorVals);
    m_map->readSensor(sensorVals);

    m_map_avg->calcAvg();

    refreshAvailable = true;
  }
    return true;
}


void Controller::initializeParameters() {
  m_adc = new SPI_ADC();
  refreshAvailable = true;

  m_map = new MAPSensor();
  m_ect = new ECTSensor();
  m_iat = new IATSensor();
  m_tps = new TPSSensor();
  m_map_avg = new SensorAvg(m_map);

  m_revCounter = RevCounter::create();
  m_speedometer = Speedometer::create();
  m_esa = EngineStateArbitrator::create(m_ect, m_revCounter);
  m_efih = EFIHardware::create();

    // Initialize AFR values.
    AFR = 0;

    // Initialize MAP and RPM indicies to zero.
    mapIndex = 0;
    rpmIndex = 0;

    // Initialize injector to disabled mode.
    // Used to detach the timer interrupt for pulsing off
    // when the engine is not running.
    constModifier = 1.0;

    // True   -> data reporting on.
    // False  -> data reporting off.
    enableSendingData = true;
    currentlySendingData = enableSendingData;
    haveInjected = false;
    if(enableSendingData) {
      SDConnected = SD.begin(BUILTIN_SDCARD);
      if(SDConnected) { // find new fileName
        int fileNumber = 0;
        do {
          sprintf(fileName, "%s%i", baseFileName, fileNumber);
          fileNumber++;
        } while(SD.exists(fileName));
      }
    }

    // Calculate base pulse times from fuel ratio table. Should actually
    // store the last table used and recall it from memory here!
    calculateBasePulseTime(false, 0, 0);

    lastRPMCalcTime = micros();
}

void Controller::onRevDetection() {
  if (m_esa->getEngineState() == EngineState::MAX_TEMP_EXCEEDED)
    return;

  if (!m_revCounter->countRevolution())
    return;

  if (m_efih->isInjDisabled()) {
    enableINJ();
  }

  //Inject on every second revolution because this is a 4 stroke engine
  if (!detectEngineOff() && inStartingRevs()) {
    if (totalRevolutions % 2 == 1)
      pulseOn();
  } 
  else {  // inject when the time since the last trough is < 1 period (2 rotations between troughs)
    if (!detectEngineOff() && (m_map_avg->getSensorGauss() > m_map->getReading()))//&& ((60 * 1E6) / RPM > micros() - MAPTrough))
      pulseOn();
  }
}


long Controller::getFuelUsed() {
  //volumetric flow rate = mass flow rate / density
  return givenFlow * totalPulseTime / density; //in mL
}


long Controller::interpolate2D(int blrow, int blcol, double x, double y) {
    // Takes the coordinate of the bottom left corner of the square to perform 2D interpolation over.
    // x and y must be given in unit form. i.e., y = (yc-y1)/(y2-y1) and x = (xc-x1)/(x2-x1)
    // (0 <= y <= 1 and 0 <= x <= 1)
    return
    injectorBasePulseTimes[blrow][blcol]*(1-y)*(1-x)+
    injectorBasePulseTimes[blrow+1][blcol]*(y)*(1-x)+
    injectorBasePulseTimes[blrow][blcol+1]*(1-y)*(x)+
    injectorBasePulseTimes[blrow+1][blcol+1]*(y)*(x);
  }


void Controller::lookupPulseTime() {
    // Map the MAP and RPM readings to the dimensionns of the AFR lookup table
    noInterrupts();

    scaledMAP = Utils::doubleMap(m_map_avg->getSensorAvg(), minMAP, maxMAP, 0, numTableRows - 1); //number from 0 - numTableRows-1
    scaledRPM = Utils::doubleMap(RPM, minRPM, maxRPM, 0, numTableCols - 1); //number from 0 - numTableCols-1

    // Clip out of bounds to the min or max value, whichever is closer.
    scaledMAP = constrain(scaledMAP, 0, numTableRows - 1);
    scaledRPM = constrain(scaledRPM, 0, numTableCols - 1);

    // Get lower bounds for load and rpm indicies.
    mapIndex = scaledMAP; // double to int
    rpmIndex = scaledRPM;

    interrupts();

    // Clip extrapolation to the value at the max index. Otherwise, perform 2D interpolation to get
    // the base pulse time and then divide by the temperature.
    long tempPulseTime;
    if (rpmIndex < numTableCols - 1 && mapIndex < numTableRows - 1) {
        // Interpolation case
        tempPulseTime = interpolate2D(mapIndex, rpmIndex, scaledMAP-mapIndex, scaledRPM-rpmIndex) / m_iat->getReading();
    }
    else {
        // Clipped case
        tempPulseTime = injectorBasePulseTimes[mapIndex][rpmIndex] / m_iat->getReading();
    }

    // Add extra fuel for starting
    if (inStartingRevs()){
        tempPulseTime *= 1.4;
    }

    noInterrupts();
    injectorPulseTime = openTime + tempPulseTime * constModifier; // ADJUST OPEN TIME
    interrupts();
}


void Controller::calculateBasePulseTime(bool singleVal, int row, int col) {
  if (singleVal) {
    // Cover the range of pressure values from min - max inclusive.
    unsigned long pressure = map(row, 0, numTableRows - 1, minMAP, maxMAP);
    // Compute a base pulse time in units of microseconds * Kelvin. Temperature will be
    // divided on the fly to get the actual pulse time used.
    injectorBasePulseTimes[row][col] = 1E6 * pressure * injectionConstant / (fuelRatioTable[row][col]);
    return;
  }

  for (int x = 0; x < numTableRows; x++) {
    for (int y = 0; y < numTableCols; y++) {
      // Cover the range of pressure values from min - max inclusive.
      unsigned long pressure = map(x, 0, numTableRows - 1, minMAP, maxMAP);
      // Compute a base pulse time in units of microseconds * Kelvin. Temperature will be
      // divided on the fly to get the actual pulse time used.
      injectorBasePulseTimes[x][y] = 1E6 * pressure * injectionConstant / (fuelRatioTable[x][y]);
    }
  }
}

void Controller::updateRPM() {
  m_revCounter->updateRPM();
}

void Controller::updateEngineState() {
  m_esa->updateEngineState();
}

void Controller::enableINJ() {
  m_efih->enableINJ();
}
void Controller::pulseOn() {
  m_efih->pulseOn();
}
void Controller::pulseOff() {
  m_efih->pulseOff();
}


bool Controller::detectEngineOff() {
  // if micros() overflows, we can tolerate defaulting to on state (will just inject less)
  //   it will be worse if the engine goes off when it is actually on which means we'll inject more
  //   (as if in starting state)
  if (micros() - previousRev > SHUTOFF_DELAY) {
    return true;
  }
  return false;
}

long Controller::getRPM (long int timePassed, int rev) {
  return (60 * 1E6 * rev) / (timePassed);
}

bool Controller::inStartingRevs(){
  return m_revCounter->getStartingRevolutions() <= NUM_REVS_FOR_START;
}