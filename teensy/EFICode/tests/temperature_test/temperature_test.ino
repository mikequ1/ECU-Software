#include "dht.h"

dht* m_dht;
long tlastRead;

#define DHT11_PIN 23

void setup(){
  Serial.begin(9600);
  m_dht = new dht();
  tlastRead = micros();
}

void loop(){
  if (micros() > tlastRead + 1E6) {
    int chk = m_dht->read11(DHT11_PIN);
    Serial.print("Temperature = ");
    Serial.println(m_dht->temperature);
    Serial.print("Humidity = ");
    Serial.println(m_dht->humidity);
    tlastRead = micros();
  }

}
