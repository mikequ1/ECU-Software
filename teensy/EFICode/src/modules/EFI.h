#ifndef EFI_H
#define EFI_H

#include "../sensors/AnlgSensor.h"
#include "../sensors/SensorAvg.h"
#include "../Constants.h"

#include "RevCounter.h"
#include "EngineStateArbitrator.h"
#include "EFIHardware.h"

class EFI {
public:
    static EFI* create(AnlgSensor* iatSensor, SensorAvg* mapAvg, RevCounter* revCounter, EngineStateArbitrator* esa, EFIHardware* efiHw);
    void onRevDetectionInject();
    long interpolate2D(long (&injectorBasePulseTimes)[numTableRows][numTableCols], int blrow, int blcol, double x, double y);
    void lookupPulseTime(long (&injectorBasePulseTimes)[numTableRows][numTableCols]);
    void calculateBasePulseTime(long (&injectorBasePulseTimes)[numTableRows][numTableCols], double (&fuelRatioTable)[numTableRows][numTableCols], bool singleVal, int row, int col);
private:
    EFI(AnlgSensor* iatSensor, SensorAvg* mapAvg, RevCounter* revCounter, EngineStateArbitrator* esa, EFIHardware* efiHw);
    double m_AFR;

    int m_mapIndex;
    int m_rpmIndex;

    double m_scaledMAP;
    double m_scaledRPM;

    AnlgSensor* m_iatSensor;
    SensorAvg* m_mapAvg;
    RevCounter* m_revCounter;
    EngineStateArbitrator* m_esa;
    EFIHardware* m_efiHw;
};

#endif