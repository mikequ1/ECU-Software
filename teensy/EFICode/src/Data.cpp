#include "Controller.h"

#include "Arduino.h"
#include "SD.h"
#include "SPI.h"

void Controller::sendCurrentData() { // THIS MUST TAKE LESS THAN 1 ms (to guaruntee micros doesn't miss an overflow)
// TODO:
// -last row and column used in tables ( maybe send back actual values used)
// -whether we are still on startup cycles
// -engine on? (or just use RPM = 0)
  char toSend [1000];
  char ecuData [500];
  //NOTE: micros() will overflow if system is on for 70 minutes!
  sprintf(ecuData, "%010u:%06i:%03.3f:%03.3f:%06.3f:%06.3f:%03.3f:%05i\n",//56 bytes
    micros(),
    totalRevolutions, 
    s_ect->getReading(), 
    s_iat->getReading(), 
    s_map->getReading(), 
    s_map_avg->getSensorAvg(),
    s_tps->getReading(), 
    RPM
  );
  
  sprintf(toSend, "%010u:%06i:%03.3f:%03.3f:%03.3f:%03.3f:%03.3f:%03.3f:%05i:%05i:%05i:%02.2f:%02.2f:%01.3f:%01i:%01i:%010u:%03.3f:%03.3f:%01i:%s:%01i:%03.3f\n", // about 97 bytes? (800-900 us)
  	micros(), 
	totalRevolutions, 
	s_ect->getReading(), 
	s_iat->getReading(), 
	s_map->getReading(), 
	s_map_avg->getSensorAvg(),
	s_tps->getReading(), 
	AFR, 
	RPM, 
  injectorPulseTime,
	lastPulse,
	scaledMAP, 
	scaledRPM,
	AFR,
	startingRevolutions <= numRevsForStart,
	haveInjected,
	s_map_avg->getTroughTime(),
  s_map_avg->getPrevD(),
  s_map_avg->getSensorGauss(),
  SDConnected,
  fileName,
  s_ect->getReading() > MAX_ALLOWABLE_ECT,
  AFR);

  if(SDConnected) { // open and write to file
    File logFile = SD.open(fileName, FILE_WRITE);
    logFile.write(toSend);
    logFile.close();
  }
  
  sendInfo(ecuData);
  Serial.write(ecuData);
  //Serial.println(ecuData);
}

bool Controller::sendInfo(char* str){
  char* i = str;
  while (*i != '\n'){
    char cur_char = *i;
    Serial1.write(cur_char);
    i++;
  }
  Serial1.write('\n');
  return true;
}


void Controller::trySendingData() {
  if (currentlySendingData) {
      sendCurrentData();
      haveInjected = false;
  }
}
