#include "EngineStateArbitrator.h"
#include "../sensors/ECTSensor.h"
#include "Arduino.h"
#include "RevCounter.h"

EngineStateArbitrator* EngineStateArbitrator::create(AnlgSensor* ectSensor, RevCounter* revCounter){
    if (!ectSensor)
        return nullptr;
    if (!revCounter)
        return nullptr;
    return new EngineStateArbitrator(ectSensor, revCounter);
}

void EngineStateArbitrator::updateEngineState() {
    if (detectEngineOff()){
        m_revCounter->handleEngineOff();
        m_engineState = OFF;
    }
    else if (inStartingRevs())
        m_engineState = STARTING;
    else if (checkMaxTemp())
        m_engineState = MAX_TEMP_EXCEEDED;
    else
        m_engineState = OK;
}

EngineState EngineStateArbitrator::getEngineState() {
    return m_engineState;
}

int EngineStateArbitrator::getEngineStateCode() {
    switch (m_engineState)
    {
    case OFF:
        return 0;
        break;
    case STARTING:
        return 2;
        break;
    case MAX_TEMP_EXCEEDED:
        return -2;
        break;
    case OK:
        return 1;
        break;
    default:
        return -1;
        break;
    }
}
    
EngineStateArbitrator::EngineStateArbitrator(AnlgSensor* ectSensor, RevCounter* revCounter) :
    m_ectSensor(ectSensor),
    m_revCounter(revCounter),
    m_engineState(OFF) {
}

bool EngineStateArbitrator::detectEngineOff(){
    if (micros() - m_revCounter->getPrevRevTime() > SHUTOFF_DELAY)
        return true;
    return false;
}

bool EngineStateArbitrator::inStartingRevs(){
   return m_revCounter->getStartingRevolutions() <= NUM_REVS_FOR_START;
}
    
bool EngineStateArbitrator::checkMaxTemp(){
    if (m_ectSensor->getReading() > MAX_ALLOWABLE_ECT)
        return true;
    return false;
}