#include "IATSensor.h"
#include "../Constants.h"
#include "math.h"
#include "Arduino.h"
#include "../utils/ECULogger.h"


IATSensor::IATSensor(){ 
    m_DHT = new dht();
    m_tLastReading = micros();
}

void IATSensor::readSensor(){
    noInterrupts();
    if (micros() > m_tLastReading + 1E6) {
        int chk = m_DHT->read11(IAT_Pin);
        int celsius = m_DHT->temperature;
        if (celsius != -999){
            m_reading = m_DHT->temperature + 273;
        }
        m_tLastReading = micros();
        LOGD("read sensor \n");
    }
    interrupts();
}

double IATSensor::getReading() {
    return m_reading;
}