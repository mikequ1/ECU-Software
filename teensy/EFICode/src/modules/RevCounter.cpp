#include "RevCounter.h"
#include "Arduino.h"

const unsigned long MIN_DELAY_PER_REV = (1 / (8000 * (1/60))) * (1E6); // in units of microseconds
const int REVS_PER_CALC = 5;

RevCounter* RevCounter::create(SensorAvg* mapAverager){
    if (!mapAverager)
        return nullptr;
    return new RevCounter(mapAverager);
}

RevCounter::RevCounter(SensorAvg* mapAverager) : 
    m_mapAverager(mapAverager),
    m_prevRevTime(0),
    m_prevRpmCalcTime(0),
    m_calcRevolutions(0),
    m_totalRevolutions(0),
    m_startingRevolutions(0),
    m_rpm(0) {
}

void RevCounter::countRevolution(){
    if ((micros() - m_prevRevTime > 0) && (micros() - m_prevRevTime < MIN_DELAY_PER_REV))
        return;
    previousRev = micros();

    m_calcRevolutions++;
    m_totalRevolutions++;
    m_startingRevolutions++;
}

void RevCounter::updateRPM(){
    noInterrupts();
    int tempRev = m_calcRevolutions; //Prevents revolutions being read while it is being modified by the
    interrupts();

    if (tempRev >= REVS_PER_CALC) {
        noInterrupts(); //To ensure that the interrupt of countRev doesn't get lost in case of bad timing of threads
        unsigned long curRPMCalcTime = micros();
        if(curRPMCalcTime - m_prevRpmCalcTime > 0) // only write if this value is positive (protect from overflow)
    	    m_rpm = getRPM(curRPMCalcTime - m_prevRpmCalcTime, tempRev);
        m_prevRpmCalcTime = curRPMCalcTime;
        m_calcRevolutions = 0; //Race Conditions Modification Problem
        interrupts();
    }
}

long RevCounter::getRPM (unsigned long timePassed, int revs){
    return (60 * 1E6 * revs) / (timePassed);
}

unsigned long RevCounter::getTotalRevolutions(){
    return m_totalRevolutions;
}
