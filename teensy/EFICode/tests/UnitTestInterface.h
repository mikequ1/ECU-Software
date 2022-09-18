#include "Arduino.h"

class UnitTestInterface {
    public:
        virtual void setup() = 0;
        virtual void verify(bool cond){
            if (cond) {
                Serial.println("### Test Passed ###");
            } else {
                Serial.println("!!! Test Failed !!!");
            }
        }
        void verifyApproxEqual(double val1, double val2, double scale){
            double comp = val1 - val2;
            if (comp > 0 && comp < scale){
                Serial.println("### Test Passed ###");
            } else if (comp < 0 && comp > scale * -1){
                Serial.println("### Test Passed ###");
            } else {
                Serial.println("!!! Test Failed !!!");
            }
        }

};