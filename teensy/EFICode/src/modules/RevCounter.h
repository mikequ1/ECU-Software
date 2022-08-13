#ifndef REVCOUNTER_H
#define REVCOUNTER_H

#include "../sensors/AnlgSensor.h"
#include "../sensors/SensorAvg.h"

class RevCounter {
public:
    static RevCounter* create();
    bool countRevolution();

    void updateRPM();
    long getRPM (unsigned long timePassed, int revs);

    void handleEngineOff();

    unsigned long getTotalRevolutions();
    unsigned long getStartingRevolutions();
    unsigned long getPrevRevTime();

private:
    RevCounter();

    unsigned long m_calcRevolutions;
    unsigned long m_totalRevolutions;
    unsigned long m_startingRevolutions;

    long m_rpm;

    unsigned long m_prevRevTime;
    unsigned long m_prevRpmCalcTime;
};

#endif