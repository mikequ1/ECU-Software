#include "Speedometer.h"

#include "Arduino.h"
#include "../Constants.h"

const double DISTANCE_PER_ROTATION = 0; // How far will the car roll in one rear wheel revolution

Speedometer* Speedometer::create(){
    return new Speedometer();
}

Speedometer::Speedometer(){
    m_totalRotations = 0;
    m_lastRotationTimestamp = micros();
}

void Speedometer::updateSpd(){
    m_totalRotations++;

    double tmp_spd = DISTANCE_PER_ROTATION/(1000 * 1000 * (micros() - m_lastRotationTimestamp)); //ft/us
    m_spd = tmp_spd / (5280 * 60 * 60);
    m_lastRotationTimestamp = micros();
}

double Speedometer::getSpd(){
    noInterrupts();
    double tmp_spd = m_spd;
    interrupts();
    return tmp_spd; 
}