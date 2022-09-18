#include "Arduino.h"
#include "../src/modules/EFI.h"
#include "../src/Constants.h"

struct mockESA {
    EngineState m_engineState;
};

struct mockRevCounter {
    long m_rpm;
};

struct mockMapAvg {

};