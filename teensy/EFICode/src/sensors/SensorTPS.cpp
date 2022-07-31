#include "SensorTPS.h"

#include "Arduino.h"
#include "../Constants.h"

const double TPS_0_DEG = 54;
const double TPS_90_DEG = 951;

SensorTPS::SensorTPS() {
    m_lastThrottleMeasurementTime = micros();
    m_TPSval = 0;
}

void SensorTPS::getTPSSensor(int* sensorVals) {
  unsigned long currThrottleMeasurementTime = micros();
  //calculate open throttle area (i think)
  //double newTPS = 1 - cos(((double(analogRead(TPS_Pin))-TPS_0Deg)/(TPS_90Deg - TPS_0Deg))*HALF_PI);

  double newTPSVal = sensorVals[TPS_CHAN];
  double newTPS = doubleMap(newTPSVal, TPS_0_DEG, TPS_90_DEG, 0, 1); //need to re-adjust TPS_0Deg and TPS_90Deg
  
  if(newTPS < 0)
    newTPS = 0;
  if(newTPS > 1)
    newTPS = 1;
  if(currThrottleMeasurementTime - m_lastThrottleMeasurementTime > 0)
    m_DTPS = (newTPS - m_TPSval) / (currThrottleMeasurementTime - m_lastThrottleMeasurementTime);
  m_lastThrottleMeasurementTime = currThrottleMeasurementTime;
  m_TPSval = newTPS;
}

double SensorTPS::getTPS() {
    return m_TPSval;
}

double SensorTPS::doubleMap(double val, double minIn, double maxIn, double minOut, double maxOut){
  return ((val - minIn) / (maxIn - minIn)) * (maxOut - minOut) + minOut;
}
