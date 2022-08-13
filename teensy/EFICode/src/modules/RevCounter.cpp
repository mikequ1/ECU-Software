#include "RevCounter.h"
#include "Arduino.h"

const unsigned long MIN_DELAY_PER_REV = (1 / (8000 * (1/60))) * (1E6); // in units of microseconds
const int REVS_PER_CALC = 5;

RevCounter* RevCounter::create(){
    return new RevCounter();
}

RevCounter::RevCounter() : 
    m_prevRevTime(micros()),
    m_prevRpmCalcTime(0),
    m_calcRevolutions(0),
    m_totalRevolutions(0),
    m_startingRevolutions(0),
    m_rpm(0) {
}

bool RevCounter::countRevolution(){
    if ((micros() - m_prevRevTime > 0) && (micros() - m_prevRevTime < MIN_DELAY_PER_REV))
        return false;
    m_prevRevTime = micros();

    m_calcRevolutions++;
    m_totalRevolutions++;
    m_startingRevolutions++;
    return true;
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

void RevCounter::handleEngineOff(){
    m_totalRevolutions = 0;
    m_startingRevolutions = 0;
    m_rpm = 0;
    m_prevRpmCalcTime = micros();
}

long RevCounter::getRPM (unsigned long timePassed, int revs){
    return (60 * 1E6 * revs) / (timePassed);
}

unsigned long RevCounter::getTotalRevolutions(){
    return m_totalRevolutions;
}

unsigned long RevCounter::getStartingRevolutions(){
    return m_startingRevolutions;
}

unsigned long RevCounter::getPrevRevTime(){
    return m_prevRevTime;
}
