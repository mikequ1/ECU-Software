#include "SensorMAP.h"

#include "Arduino.h"

SensorMAP::SensorMAP(){
    m_MAPAvg = new NoiseReduced(100);
    m_MAPval = 0;
    m_prevdMAP = 0;
    m_prevMAP = 0;
    m_tMAPpeak = 0;
    m_tMAPtrough = 0;
    m_tMAPupdate = 0;
}


double SensorMAP::getMap() {
  return m_MAPval;
}

double SensorMAP::getMAPSensor(int* sensorVals) {
  //Calculates MAP, outputs in Pa
  return MAPConversion * sensorVals[MAP_CHAN] + MAPOffset;
}

double SensorMAP::getMapGauss(){
  return m_MAPAvg->getGauss();
}

double SensorMAP::getMapAvg(){
  return m_MAPAvg->getData();
}

unsigned long SensorMAP::getMAPPeak(){
  return m_tMAPpeak;
}

unsigned long SensorMAP::getMAPTrough(){
  return m_tMAPtrough;
}

double SensorMAP::getPrevdMAP(){
  return m_prevdMAP;
}

double SensorMAP::verifyMAP(double val){
  if (val > 300000 | val < -100000)
    return -1.0;
  return val;
}

void SensorMAP::calcMAPAvg(int* sensorVals){
  m_MAPval = getMAPSensor(sensorVals);

  m_MAPAvg->addData(m_MAPval);
  // Update MAPPeak and MAPTrough
  if(m_tMAPupdate - micros() > minMAPdt) {
    double dMAP = m_MAPAvg->getGauss() - m_prevMAP;
    if((m_prevdMAP < 0) != (dMAP < 0)) { // if slopes have different sign
      if(dMAP < 0)
        m_tMAPpeak = micros();
      else
        m_tMAPtrough = micros();
	  }
    m_prevdMAP = dMAP;
		m_prevMAP = m_MAPAvg->getGauss();
	  m_tMAPupdate = micros();
  }
}
