#ifndef TPSSENSOR_H
#define TPSSENSOR_H

#include "AnlgSensor.h"

class TPSSensor : public AnlgSensor {
public:
    TPSSensor();
    void readSensor(const int* sensorVals);
    double getReading();
private:
    double m_reading;
    double m_dTPS;
    unsigned long m_lastThrottleMeasurementTime;
};

#endif