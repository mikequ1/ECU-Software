#ifndef SENSORAVG_H
#define SENSORAVG_H

#include "AnlgSensor.h"
#include "../utils/NoiseReduced.h"

class SensorAvg {
public:
    SensorAvg(AnlgSensor* sn);
    
    double getSensorGauss();
    double getSensorAvg();
    unsigned long getPeakTime();
    unsigned long getTroughTime();
    double getPrev();
    double getPrevD();

    double getSensorReading();
    void calcAvg();

private:
    double m_prevD;
    double m_prev;
    unsigned long m_peakTime;
    unsigned long m_troughTime;
    unsigned long m_updateTime;

    AnlgSensor* m_sensor;
    NoiseReduced* m_avg;
};

#endif