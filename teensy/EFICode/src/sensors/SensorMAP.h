#ifndef SENSORMAP_H
#define SENSORMAP_H

#include "../utils/NoiseReduced.h"
#include "../Constants.h"

// MPX4115A MAP sensor calibration
const double MAPVs = Vs_5;
const double MAPDelta = 0.045; // should be between +/- 0.0675 volts (1.5 * 0.009 * Vs where Vs is 5)
const double MAPSlope = 1E3/(MAPVs*0.009);  //Pa / Volt
const double MAPOffset = 1E3*MAPDelta/(MAPVs*0.009) + 1E3*0.095/0.009;   //Pa
const double MAPConversion = MAPSlope * voltConv;    // Pascals / 1023
const unsigned long minMAPdt = 4000; // in microseconds


class SensorMAP {
    public:
        SensorMAP();
        //Fetch MAP's last stored value
        double getMap();

        //Fetch a new MAP value from the sensor
        double getMAPSensor(int* sensorVals);

        //MAP Averaging Getter Functions
        double getMapGauss();
        double getMapAvg();
        unsigned long getMAPPeak();
        unsigned long getMAPTrough();
        double getPrevdMAP();
        double verifyMAP(double val); //filtering out MAP values that don't make sense (UNUSED)
        
        //Reads the MAP Sensor's values and performs averaging
        void calcMAPAvg(int* sensorVals);

    private:
        double m_MAPval;

        double m_prevdMAP;
        double m_prevMAP;
        unsigned long m_tMAPupdate; // time of MAP update
        unsigned long m_tMAPpeak; // time of previous peak
        unsigned long m_tMAPtrough; // time of previous trough

        NoiseReduced* m_MAPAvg;
};

#endif
