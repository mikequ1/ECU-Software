#ifndef REVCOUNTER_H
#define REVCOUNTER_H

#include "../sensors/AnlgSensor.h"
#include "../sensors/SensorAvg.h"

class RevCounter {
public:
    /**
     * @brief Create a RevCounter instance
     * 
     * @return pointer to the newly created RevCounter object
     */
    static RevCounter* create();


    /**
     * @brief Increment the revolution count of the engine
     * 
     * @return true if the operation is successful
     * @return false if micros() has overflown, or if a double count occurs
     */
    bool countRevolution();

    /**
     * @brief Update the RPM reading based on the intervals of engine revs
     * 
     */
    void updateRPM();


    /**
     * @brief Routine to reset the state of the RevCounter when the engine is detected to be off
     * 
     */
    void handleEngineOff();


    /**
     * @brief Get the Total Revolutions count
     * 
     * @return unsigned long, the total number of engine revolutions
     */
    unsigned long getTotalRevolutions();

    
    /**
     * @brief Get the Starting Revolutions count
     * 
     * @return unsigned long, the number of starting revolutions
     */
    unsigned long getStartingRevolutions();


    /**
     * @brief Get the time the most recent engine revolution was recorded
     * 
     * @return unsigned long, the time for the most recent flywheel hall sensor hit
     */
    unsigned long getPrevRevTime();


    /**
     * @brief Get the RPM value
     * 
     * @return long, the RPM value
     */
    long getRPM();

private:
    /**
     * @brief Construct a new Rev Counter object
     * 
     */
    RevCounter();


    /**
     * @brief Helper function to calculate the RPM using the number of revolutions over time
     * 
     * @param timePassed a period of time between two arbitrary flywheel hall sensor hits
     * @param revs the number of flywheel hall sensor hits over the time elapsed
     * @return long, the current RPM
     */
    long calcRPM (unsigned long timePassed, int revs);


    // The number of revolutions for RPM calculation
    unsigned long m_calcRevolutions;
    // The number of revolutions since the electrical system was turned on
    unsigned long m_totalRevolutions;
    // The number of revolutions since the engine was last on
    unsigned long m_startingRevolutions;
    // The engine's current RPM at flywheel
    long m_rpm;

    // The timestamp of the most recent hall sensor hit
    unsigned long m_prevRevTime;
    // The timestamp of the most recent RPM calculation
    unsigned long m_prevRpmCalcTime;
};

#endif