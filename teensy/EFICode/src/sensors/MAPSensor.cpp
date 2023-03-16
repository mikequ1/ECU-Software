#include "MAPSensor.h"
#include "../Constants.h"
#include "Arduino.h"

// MPX4115A MAP sensor calibration
// const double MAPVs = Vs_5;
// const double MAPDelta = 0.045; // should be between +/- 0.0675 volts (1.5 * 0.009 * Vs where Vs is 5)
// const double MAPSlope = 1E3/(MAPVs*0.009);  //Pa / Volt
// const double MAPOffset = 1E3*MAPDelta/(MAPVs*0.009) + 1E3*0.095/0.009;   //Pa
// const double MAPConversion = MAPSlope * voltConv;    // Pascals / 1023

const double  MAP_VS = Vs_5;

void MAPSensor::readSensor(const int* sensorVals){
    noInterrupts();
    // m_reading = MAPConversion * sensorVals[MAP_CHAN] + MAPOffset;
    m_reading = ((((sensorVals[MAP_CHAN]  *  voltConv)/MAP_VS) - 0.04)  * 100 *  1000/9 ) -381 +  101325;
    interrupts();
}

double MAPSensor::getReading() {
    return m_reading;
}