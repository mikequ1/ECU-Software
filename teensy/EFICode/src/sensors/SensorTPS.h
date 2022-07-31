#ifndef SENSORTPS_H
#define SENSORTPS_H

class SensorTPS {
    public:
        SensorTPS();
        void getTPSSensor(int* sensorVals);
        double doubleMap(double val, double minIn, double maxIn, double minOut, double maxOut);
        double getTPS();
    private:
        double m_TPSval;
        double m_DTPS;
        unsigned long m_lastThrottleMeasurementTime;
};

#endif