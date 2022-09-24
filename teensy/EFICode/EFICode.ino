//EFICode_2022-23

#include "src/Controller.h"

#include "src/utils/TimerThree.h"
#include "src/utils/TimerOne.h"
//#include "EEPROM.h"

//Timer3 responsible for pulseOff, Timer1 responsible for sending data

Controller *c;

void setup() {
  // Construct Controller object.
  c = new Controller();

  // Update all sensor values to current values.
  c->updateRPM();
  c->readSensors();

  // For some reason, the internal interrupt flags can end up defaulting
  // to a triggered state before they are attached. This causes them
  // to trigger once right when they are attached. Our current workaround
  // is to attach the interrupt to a dummy function first that triggers
  // if the interrupt is already set. Then, it is safe to attach the normal interrupt.

  // Attach rpm detector to revolution counter interrupt.
  attachInterrupt(digitalPinToInterrupt(HES_Pin), dummy, RISING);
  detachInterrupt(digitalPinToInterrupt(HES_Pin));
  attachInterrupt(digitalPinToInterrupt(HES_Pin), countRev, RISING);


  // Initialize pulseOff timer.
  Timer3.initialize(1000000);
  // Attach the interrupt for INJ pulse modulation.
  Timer3.attachInterrupt(dummy);
  Timer3.detachInterrupt();
  Timer3.attachInterrupt(handle_pulseTimerTimeout);
  // Immediately stop the timer.
  Timer3.stop();


  // Attach the interrupt for data sending  
  Timer1.attachInterrupt(dummy);
  Timer1.detachInterrupt();
  Timer1.attachInterrupt(handle_sendData);
  // Set how often data is sent
  Timer1.setPeriod(DATA_RATE_PERIOD);
  Timer1.start();
  
  pinMode(LED_1, OUTPUT);  
}


void loop() {
  //Update RPM if needed.
  c->updateRPM();

  // Update Controller with most recent sensor values.
  c->readSensors();

  // Look up injection time on each loop cycle
  c->lookupPulseTime();

  // Update state of the engine
  c->updateEngineState();
}

void countRev() {
  c->onRevDetection();
}

void handle_pulseTimerTimeout() {
  if(digitalRead(LED_1) == 0){ 
    digitalWrite(LED_1, HIGH);
  } else {
    digitalWrite(LED_1, LOW);
  }
  c->pulseOff();
}

void handle_sendData() {
  // Attempt to send sensor data to the DAQ system. Will only occur if the
  // currentlySendingData flag is set to true. This flag is set by sending 
  // the ID 1 signal to the controller.
  c->trySendingData();
  Timer1.start();
}

void dummy() {}
