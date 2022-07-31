#include "IATSensor.h"
#include "../Constants.h"
#include "math.h"

// The following constants are to complete the following eq for temperature
// Temp = tempBeta / (ln(R) + (tempBeta/T_0 - lnR_0)) //	where R is the resistance of the sensor (found using voltage divider) //	eq from: https://en.wikipedia.org/wiki/Thermistor#B_or_%CE%B2_parameter_equation //
const double TEMP_BETA_IAT = 3988; // tolerance: {+/-1%,+/-1.5%}
const double T_0 = 298.15; // temp in Kelvin at which R_0 values are taken
const double lnR_0_IAT = 9.21034; //8.45531}; // {ln(10000 (10000 +/-1%)),ln(4700 (4559 to 4841))}
const double tempConstIAT = TEMP_BETA_IAT/T_0 - lnR_0_IAT;
const double R_DIV_IAT = 10000; // resistance of other resistor in voltage divider

void IATSensor::readSensor(int* sensorVals){
    double tempR = R_DIV_IAT / (maxADC/sensorVals[ECT_CHAN] - 1);
    m_reading = TEMP_BETA_IAT / (log(tempR) + tempConstIAT);
}

double IATSensor::getReading() {
    return m_reading;
}