#ifndef IATSENSOR_H
#define IATSENSOR_H

#include "AnlgSensor.h"

class IATSensor : public AnlgSensor {
public:
    void readSensor(int* sensorVals);
    double getReading();
private:
    double m_reading;
};

#endif