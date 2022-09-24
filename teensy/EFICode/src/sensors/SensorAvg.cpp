#include "SensorAvg.h"
#include "Arduino.h"

const unsigned long minDt = 4000; // in microseconds

SensorAvg::SensorAvg(AnlgSensor* sn){
    m_avg = new NoiseReduced(100);
    m_sensor = sn;

    m_prev = 0;
    m_prevD = 0;

    m_peakTime = 0;
    m_troughTime = 0;
    m_updateTime = 0;
}

double SensorAvg::getSensorGauss(){
    return m_avg->getGauss();
}

double SensorAvg::getSensorAvg(){
    return m_avg->getData();
}

unsigned long SensorAvg::getPeakTime(){
    return m_peakTime;
}

unsigned long SensorAvg::getTroughTime(){
    return m_troughTime;
}

double SensorAvg::getPrev(){
    return m_prev;
}

double SensorAvg::getPrevD(){
    return m_prevD;
}

void SensorAvg::calcAvg(){
    noInterrupts();
    m_avg->addData(m_sensor->getReading());
    // Update MAPPeak and MAPTrough
    if (m_updateTime - micros() > minDt) {
        double dMAP = m_avg->getGauss() - m_prev;
        if ((m_prev < 0) != (dMAP < 0)) { // if slopes have different sign
            if (dMAP < 0)
                m_peakTime = micros();
            else
                m_troughTime = micros();
	    }
        m_prevD = dMAP;
		m_prev = m_avg->getGauss();
	    m_updateTime = micros();
    }
    interrupts();
}

double SensorAvg::getSensorReading(){
    return m_sensor->getReading();
}