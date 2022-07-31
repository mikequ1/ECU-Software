#include "ECTSensor.h"
#include "../Constants.h"
#include "math.h"

// The following constants are to complete the following eq for temperature
// Temp = tempBeta / (ln(R) + (tempBeta/T_0 - lnR_0)) //	where R is the resistance of the sensor (found using voltage divider) //	eq from: https://en.wikipedia.org/wiki/Thermistor#B_or_%CE%B2_parameter_equation //
const double TEMP_BETA_ECT = 3988; // tolerance: {+/-1%,+/-1.5%}
const double T_0 = 298.15; // temp in Kelvin at which R_0 values are taken
const double lnR_0_ECT = 8.4849;
const double tempConstECT = TEMP_BETA_ECT/T_0 - lnR_0_ECT;
const double R_DIV_ECT = 10000; // resistance of other resistor in voltage divider

void ECTSensor::readSensor(int* sensorVals){
    double tempR = R_DIV_ECT / (maxADC/sensorVals[ECT_CHAN] - 1);
    m_reading = TEMP_BETA_ECT / (log(tempR) + tempConstECT);
}

double ECTSensor::getReading() {
    return m_reading;
}
