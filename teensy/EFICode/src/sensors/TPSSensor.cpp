#include "TPSSensor.h"
#include "Arduino.h"
#include "../Constants.h"
#include "../Utils.h"

const double TPS_0_DEG = 54;
const double TPS_90_DEG = 951;

TPSSensor::TPSSensor() {
    m_lastThrottleMeasurementTime = micros();
    m_reading = 0;
}

void TPSSensor::readSensor(const int* sensorVals) {
  noInterrupts();
  unsigned long currThrottleMeasurementTime = micros();

  double newTPSVal = sensorVals[TPS_CHAN];
  double newTPS = Utils::doubleMap(newTPSVal, TPS_0_DEG, TPS_90_DEG, 0, 1); //need to re-adjust TPS_0Deg and TPS_90Deg
  
  if(newTPS < 0)
    newTPS = 0;
  if(newTPS > 1)
    newTPS = 1;

  if(currThrottleMeasurementTime - m_lastThrottleMeasurementTime > 0)
    m_dTPS = (newTPS - m_reading) / (currThrottleMeasurementTime - m_lastThrottleMeasurementTime);
  m_lastThrottleMeasurementTime = currThrottleMeasurementTime;

  m_reading = newTPS;
  interrupts();
}

double TPSSensor::getReading() {
    return m_reading;
}

