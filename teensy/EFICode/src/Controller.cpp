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
    //Serial1.begin(BAUD_RATE);

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
    m_iat->readSensor();
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

  m_afrLoader = AFRLoader::create();
  m_revCounter = RevCounter::create();
  m_speedometer = Speedometer::create();
  m_esa = EngineStateArbitrator::create(m_ect, m_revCounter);
  m_efih = EFIHardware::create();
  m_efi = EFI::create(m_iat, m_map_avg, m_revCounter, m_esa, m_efih);
  
  //m_afrLoader->dumpAFRList();
  m_afrLoader->getFuelRatioTable(m_fuelRatioTable);
  m_afrLoader->dumpFuelRatios();

  // True   -> data reporting on.
  // False  -> data reporting off.
  enableSendingData = true;
  m_haveInjected = false;
  currentlySendingData = enableSendingData;
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
  m_efi->calculateBasePulseTime(injectorBasePulseTimes, m_fuelRatioTable, false, 0, 0);
  this->dumpBasePulseTimes();
}

void Controller::onRevDetection() {
  if (m_esa->getEngineState() == EngineState::MAX_TEMP_EXCEEDED)
    return;

  if (m_efih->isInjDisabled()) {
    enableINJ();
  }

  if (!m_revCounter->countRevolution())
    return;

  m_efi->onRevDetectionInject();
}


void Controller::lookupPulseTime() {
  m_efi->lookupPulseTime(injectorBasePulseTimes);
}

void Controller::updateRPM() {
  m_revCounter->updateRPM();
}

void Controller::enableINJ() {
  m_efih->enableINJ();
}

void Controller::pulseOff() {
  m_efih->pulseOff();
  m_haveInjected = true;
}

void Controller::updateEngineState(){
  m_efi->updateEngineState();
}

void Controller::dumpBasePulseTimes() const {
  Serial.println("===== Dumping current base pulse times =====");
    for (int i = 0; i < numTableRows; i++) {
        for (int j = 0; j < numTableCols; j++) {
            Serial.print(injectorBasePulseTimes[i][j]);
            Serial.print(", ");
        }
        Serial.println();
    }
  Serial.println("===== base pulse times finished =====");
}