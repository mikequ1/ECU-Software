#ifndef MAPSENSOR_H
#define MAPSENSOR_H

#include "AnlgSensor.h"

class MAPSensor : public AnlgSensor {
public:
    void readSensor(int* sensorVals);
    double getReading();
private:
    double m_reading;
};

#endif