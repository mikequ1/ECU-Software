#include "Arduino.h"
#include "UnitTestInterface.h"
#include "../src/modules/RevCounter.h"
#include "../src/Constants.h"

static const unsigned long MIN_DELAY_PER_REV_MS = (1 / (8000 * (1/60))) * (1E3); // in units of microseconds or 7500 us
static const int REVS_PER_CALC = 5;
static const int NUM_TEST_CYCLES = 10;

class RevCounterTest:UnitTestInterface {
public:
    void setup(){
        m_revCounter = RevCounter::create();
    }
    void countRevolutionsTest(){
        setup();
        for (int i = 0; i < NUM_TEST_CYCLES; i++){
            m_revCounter->countRevolution();
            deelay(MIN_DELAY_PER_REV_MS * 2);
        }
        bool cond = (m_revCounter->getTotalRevolutions() == 10);
        verify(cond);
    }
    void minDelayTest(){
        setup();
        for (int i = 0; i < NUM_TEST_CYCLES; i++){
            m_revCounter->countRevolution();
            delay(MIN_DELAY_PER_REV_MS * 2.5 / 10);
        }
        bool cond = (m_revCounter->getTotalRevolutions() == 2);
        verify(cond);
    }
    void calcRPMTest(){
        setup();
        for (int i = 0; i < NUM_TEST_CYCLES; i++){
            m_revCounter->countRevolution();
            m_revCounter->updateRPM();
            deelay(MIN_DELAY_PER_REV_MS * 2);
        }
        verifyApproxEqual(m_revCounter->getTotalRevolutions(), 4000.0);
    }
    
private:
    RevCounter* m_revCounter;
};