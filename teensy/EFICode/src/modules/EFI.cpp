#include "EFI.h"

#include "../Constants.h"
#include "../Utils.h"
#include "Arduino.h"

EFI* EFI::create(AnlgSensor* iatSensor, 
        SensorAvg* mapAvg, 
        RevCounter* revCounter, 
        EngineStateArbitrator* esa, 
        EFIHardware* efiHw) 
{
    if (!iatSensor) {
        return nullptr;
    }
    if (!mapAvg) {
        return nullptr;
    }
    if (!revCounter) {
        return nullptr;
    }
    if (!esa) {
        return nullptr;
    }
    if (!efiHw) {
        return nullptr;
    }
    return new EFI(iatSensor, mapAvg, revCounter, esa, efiHw);
}

EFI::EFI(AnlgSensor* iatSensor, 
        SensorAvg* mapAvg, 
        RevCounter* revCounter, 
        EngineStateArbitrator* esa, 
        EFIHardware* efiHw) :
    m_iatSensor{iatSensor},
    m_mapAvg{mapAvg},
    m_revCounter{revCounter},
    m_esa{esa},
    m_efiHw{efiHw} {
}

void EFI::onRevDetectionInject() {
    // When engine is in the starting state, start injection on every other hall effect sensor hit
    if (m_esa->getEngineState() == EngineState::STARTING){
        if (m_revCounter->getTotalRevolutions() % 2 == 1){
            m_efiHw->pulseOn();
        }
    } 
    // When engine is in normal operation state, start injection when air is flowing into the engine (i.e. MAP avg > MAP instantaneous)
    else if (m_esa->getEngineState() == EngineState::OK){
        if (m_mapAvg->getSensorGauss() > m_mapAvg->getSensorReading()){
            m_efiHw->pulseOn();
        }
    }
}

long EFI::interpolate2D(long (&injectorBasePulseTimes)[numTableRows][numTableCols], int blrow, int blcol, double x, double y) {
    // Takes the coordinate of the bottom left corner of the square to perform 2D interpolation over.
    // x and y must be given in unit form. i.e., y = (yc-y1)/(y2-y1) and x = (xc-x1)/(x2-x1)
    // (0 <= y <= 1 and 0 <= x <= 1)
    return
    injectorBasePulseTimes[blrow][blcol]*(1-y)*(1-x)+
    injectorBasePulseTimes[blrow+1][blcol]*(y)*(1-x)+
    injectorBasePulseTimes[blrow][blcol+1]*(1-y)*(x)+
    injectorBasePulseTimes[blrow+1][blcol+1]*(y)*(x);
}

void EFI::lookupPulseTime(long (&injectorBasePulseTimes)[numTableRows][numTableCols]) {
    // Map the MAP and RPM readings to the dimensionns of the AFR lookup table
    noInterrupts();
    m_scaledMAP = Utils::doubleMap(m_mapAvg->getSensorAvg(), minMAP, maxMAP, 0, numTableRows - 1); //number from 0 - numTableRows-1
    m_scaledRPM = Utils::doubleMap(m_revCounter->getRPM(), minRPM, maxRPM, 0, numTableCols - 1); //number from 0 - numTableCols-1

    // Clip out of bounds to the min or max value, whichever is closer.
    m_scaledMAP = constrain(m_scaledMAP, 0, numTableRows - 1);
    m_scaledRPM = constrain(m_scaledRPM, 0, numTableCols - 1);

    // Get lower bounds for load and rpm indicies.
    m_mapIndex = m_scaledMAP; // double to int
    m_rpmIndex = m_scaledRPM;
    interrupts();


    // Clip extrapolation to the value at the max index. Otherwise, perform 2D interpolation to get
    // the base pulse time and then divide by the temperature.
    long tempPulseTime;
    if (m_rpmIndex < numTableCols - 1 && m_mapIndex < numTableRows - 1) {
        tempPulseTime = interpolate2D(injectorBasePulseTimes, m_mapIndex, m_rpmIndex, m_scaledMAP - m_mapIndex, m_scaledRPM - m_rpmIndex) / m_iatSensor->getReading();
    } else {
        tempPulseTime = injectorBasePulseTimes[m_mapIndex][m_rpmIndex] / m_iatSensor->getReading();
    }

    // Add extra fuel for starting
    if (m_esa->getEngineState() == EngineState::STARTING){
        tempPulseTime *= 1.4;
    }

    noInterrupts();
    m_efiHw->setInjectorPulseTime(openTime + tempPulseTime); // ADJUST OPEN TIME
    interrupts();
}

void EFI::calculateBasePulseTime(long (&injectorBasePulseTimes)[numTableRows][numTableCols], double (&fuelRatioTable)[numTableRows][numTableCols], bool singleVal, int row, int col) {
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