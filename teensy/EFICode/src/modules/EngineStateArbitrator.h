#ifndef ESA_H
#define ESA_H

#include "../Constants.h"
#include "../sensors/AnlgSensor.h"
#include "RevCounter.h"

class EngineStateArbitrator {
public:
    static EngineStateArbitrator* create(AnlgSensor* ectSensor, RevCounter* revCounter);
    void updateEngineState();
    EngineState getEngineState();
private:
    EngineStateArbitrator(AnlgSensor* ECTSensor, RevCounter* RevCounter);
    bool detectEngineOff();
    bool inStartingRevs();
    bool checkMaxTemp();
    AnlgSensor* m_ectSensor;
    RevCounter* m_revCounter;
    EngineState m_engineState;
};


#endif