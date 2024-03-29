#include "EFIHardware.h"
#include "Arduino.h"

#include "../utils/TimerThree.h"
#include "../Constants.h"

EFIHardware* EFIHardware::create(){
    return new EFIHardware();
}

EFIHardware::EFIHardware() : 
    m_injIsDisabled{true},
    m_totalPulseTime{0},
    m_injectorPulseTime{0},
    m_lastPulseTime{0},
    m_pulseCount{0} {
        digitalWrite(INJ_Pin, LOW);
}

void EFIHardware::pulseOn(){
    if (m_injectorPulseTime > 2.5E5)
        m_injectorPulseTime = 2.5E5;
    Serial.println(m_injectorPulseTime);
    Timer3.setPeriod(m_injectorPulseTime);
    digitalWrite(INJ_Pin, HIGH);
    Timer3.start();

    noInterrupts(); //To ensure when lastPulse is used in pulseOff(), it isn't read as lastPulse is getting modified
    m_lastPulseTime = micros(); //Race Conditions Problem
    m_pulseCount++;
    interrupts();
}

void EFIHardware::pulseOff(){
    digitalWrite(INJ_Pin, LOW);
    Timer3.stop();

    m_totalPulseTime += (micros() - m_lastPulseTime);
}

void EFIHardware::enableINJ(){
    m_injIsDisabled = false;
}

void EFIHardware::disableINJ(){
    digitalWrite(INJ_Pin, LOW);
    Timer3.stop();
    noInterrupts();
    m_injIsDisabled = true;
    interrupts();
}

bool EFIHardware::isInjDisabled() const {
    return m_injIsDisabled;
}

void EFIHardware::setInjectorPulseTime(unsigned long injectorPulseTime) {
    m_injectorPulseTime = injectorPulseTime;
}

//TODO
unsigned long EFIHardware::getTotalFuelUsed() const {
    return INJ_GIVEN_FLOW * m_totalPulseTime / INJ_DENSITY; //in mL
}

int EFIHardware::getPulseCount() const {
    return m_pulseCount;
}