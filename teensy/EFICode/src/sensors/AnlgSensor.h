#ifndef ANLGSENSOR_H
#define ANLGSENSOR_H

//abstract class that represents all analog reading sensors
class AnlgSensor {

public:
    /**
     * @brief Construct a new Anlg Sensor object
     * 
     */
    AnlgSensor() {}


    /**
     * @brief Destroy the Anlg Sensor object
     * 
     */
    virtual ~AnlgSensor() {}


    /**
     * @brief Obtain the reading for a specific sensor from the ADC and save it to a usable value
     * 
     * @param sensorVals an array that represents a collection of available channels on the ADC
     */
    virtual void readSensor(const int* sensorVals) = 0;


    /**
     * @brief Get the Reading for the current sensor
     * 
     * @return the numerical reading of the current sensor
     */
    virtual double getReading() = 0;

private:
    // The reading of the corresponding sensor
    double m_reading;
};

#endif