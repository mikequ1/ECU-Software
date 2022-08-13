#include "MAPSensor.h"
#include "../Constants.h"

// MPX4115A MAP sensor calibration
const double MAPVs = Vs_5;
const double MAPDelta = 0.045; // should be between +/- 0.0675 volts (1.5 * 0.009 * Vs where Vs is 5)
const double MAPSlope = 1E3/(MAPVs*0.009);  //Pa / Volt
const double MAPOffset = 1E3*MAPDelta/(MAPVs*0.009) + 1E3*0.095/0.009;   //Pa
const double MAPConversion = MAPSlope * voltConv;    // Pascals / 1023

void MAPSensor::readSensor(const int* sensorVals){
    m_reading = MAPConversion * sensorVals[MAP_CHAN] + MAPOffset;
}

double MAPSensor::getReading() {
    return m_reading;
}