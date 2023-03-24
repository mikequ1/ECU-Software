#ifndef EFI_H
#define EFI_H

#include "../sensors/IATSensor.h"
#include "../sensors/SensorAvg.h"
#include "../Constants.h"

#include "RevCounter.h"
#include "EngineStateArbitrator.h"
#include "EFIHardware.h"

class EFI {
public:
    static EFI* create(IATSensor* iatSensor, SensorAvg* mapAvg, RevCounter* revCounter, EngineStateArbitrator* esa, EFIHardware* efiHw);
    void onRevDetectionInject();
    long interpolate2D(long (&injectorBasePulseTimes)[numTableRows][numTableCols], int blrow, int blcol, double x, double y);
    void lookupPulseTime(long (&injectorBasePulseTimes)[numTableRows][numTableCols]);
    void calculateBasePulseTime(long (&injectorBasePulseTimes)[numTableRows][numTableCols], double (&fuelRatioTable)[numTableRows][numTableCols], bool singleVal, int row, int col);
    void updateEngineState();
private:
    EFI(IATSensor* iatSensor, SensorAvg* mapAvg, RevCounter* revCounter, EngineStateArbitrator* esa, EFIHardware* efiHw);
    double m_AFR;

    int m_mapIndex;
    int m_rpmIndex;

    double m_scaledMAP;
    double m_scaledRPM;

    IATSensor* m_iatSensor;
    SensorAvg* m_mapAvg;
    RevCounter* m_revCounter;
    EngineStateArbitrator* m_esa;
    EFIHardware* m_efiHw;
};

#endif