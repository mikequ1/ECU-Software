#ifndef IATSENSOR_H
#define IATSENSOR_H

#include "AnlgSensor.h"
#include "dht.h"

class IATSensor {
public:
    IATSensor();
    void readSensor();
    double getReading();

private:
    double m_reading;
    dht* m_DHT;
    long m_tLastReading;
};

#endif