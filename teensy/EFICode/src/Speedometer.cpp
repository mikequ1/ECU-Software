#include "Speedometer.h"

#include "Arduino.h"
#include "Constants.h"

Speedometer::Speedometer(){
    totalRotations = 0;
    lastRotationTimestamp = micros();
}

void Speedometer::updateSpd(){
    totalRotations++;
    double tmp_spd = DISTANCE_PER_ROTATION/(1000 * 1000 * (micros() - lastRotationTimestamp)); //ft/us
    spd = tmp_spd / (5280 * 60 * 60);
    lastRotationTimestamp = micros();
}

double Speedometer::getSpd(){
    noInterrupts();
    double tmp_spd = spd;
    interrupts();
    return tmp_spd; 
}