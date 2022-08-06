#ifndef REVCOUNT_H
#define REVCOUNT_H

#include "../sensors/AnlgSensor.h"
#include "../sensors/SensorAvg.h"

class RevCount {
public:
    RevCount* create(AnlgSensor* mapSensor, SensorAvg* mapAverager);
    void countRevolution();

    void updateRPM();
    long getRPM (unsigned long timePassed, unsigned long revs);

    unsigned long getTotalRevolutions();

private:
    AnlgSensor* m_mapSensor;
    SensorAvg* m_mapAverager;

    unsigned long m_calcRevolutions;
    unsigned long m_totalRevolutions;
    unsigned long m_startingRevolutions;

    long m_rpm;

    unsigned long m_prevRevTime;
    unsigned long m_prevRpmCalcTime;
};

#endif