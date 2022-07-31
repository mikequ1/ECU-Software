#ifndef ECTSENSOR_H
#define ECTSENSOR_H

#include "AnlgSensor.h"

class ECTSensor : public AnlgSensor {
public:
    void readSensor(int* sensorVals);
    double getReading();
private:
    double m_reading;
};

#endif