#ifndef REVCOUNTER_H
#define REVCOUNTER_H

#include "../sensors/AnlgSensor.h"
#include "../sensors/SensorAvg.h"

class RevCounter {
public:
    RevCounter* create(SensorAvg* mapAverager);
    void countRevolution();

    void updateRPM();
    long getRPM (unsigned long timePassed, int revs);

    unsigned long getTotalRevolutions();

private:
    RevCounter(SensorAvg* mapAverager);

    SensorAvg* m_mapAverager;

    unsigned long m_calcRevolutions;
    unsigned long m_totalRevolutions;
    unsigned long m_startingRevolutions;

    long m_rpm;

    unsigned long m_prevRevTime;
    unsigned long m_prevRpmCalcTime;
};

#endif