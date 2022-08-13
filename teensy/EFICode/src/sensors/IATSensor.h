#ifndef IATSENSOR_H
#define IATSENSOR_H

#include "AnlgSensor.h"

class IATSensor : public AnlgSensor {
public:
    void readSensor(const int* sensorVals);
    double getReading();
private:
    double m_reading;
};

#endif