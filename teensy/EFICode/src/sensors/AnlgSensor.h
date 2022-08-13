#ifndef ANLGSENSOR_H
#define ANLGSENSOR_H

//abstract class that represents all analog reading sensors
class AnlgSensor {

public:
    AnlgSensor() {}
    virtual ~AnlgSensor() {}
    virtual void readSensor(const int* sensorVals) = 0;
    virtual double getReading() = 0;

private:
    double m_reading;
};

#endif