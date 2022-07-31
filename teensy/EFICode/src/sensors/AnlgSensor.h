#ifndef ANLGSENSOR_H
#define ANLGSENSOR_H

//singleton abstract class that represents all analog reading sensors
class AnlgSensor {

public:
    AnlgSensor() {}
    virtual ~AnlgSensor() {}
    virtual void readSensor(int* sensorVals) = 0;
    virtual double getReading() = 0;

private:
    double m_reading;
};

#endif