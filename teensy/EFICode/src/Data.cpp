#include "Controller.h"

#include "Arduino.h"
#include "SD.h"
#include "SPI.h"

#include "utils/ECULogger.h"

void Controller::sendCurrentData() { // THIS MUST TAKE LESS THAN 1 ms (to guaruntee micros doesn't miss an overflow)
// TODO:
// -last row and column used in tables ( maybe send back actual values used)
// -whether we are still on startup cycles
// -engine on? (or just use RPM = 0)
  //char toSend [1000];
  char ecuData [500];
  //NOTE: micros() will overflow if system is on for 70 minutes!
  // sprintf(ecuData, "%010lu >> %06lu : %05ld | %03.3f:%03.3f:%06.3f:%06.3f:%03.3f | %01d:%02d:%01d\n",
  //   micros(),
  //   m_revCounter->getTotalRevolutions(), 
  //   m_revCounter->getRPM(),
  //   m_ect->getReading(), 
  //   m_iat->getReading(), 
  //   m_map->getReading(), 
  //   m_map_avg->getSensorAvg(),
  //   m_tps->getReading(),
  //   m_esa->getEngineStateCode(),
  //   m_efih->getPulseCount(),
  //   m_efih->isInjDisabled()
  // );

  LOGS("%010lu >> %06lu : %05ld | %03.3f:%03.3f:%06.3f:%06.3f:%03.3f | %01d:%02d:%01d\n",
    micros(),
    m_revCounter->getTotalRevolutions(), 
    m_revCounter->getRPM(),
    m_ect->getReading(), 
    m_iat->getReading(), 
    m_map->getReading(), 
    m_map_avg->getSensorAvg(),
    m_tps->getReading(),
    m_esa->getEngineStateCode(),
    m_efih->getPulseCount(),
    m_efih->isInjDisabled()
  );
  
  // sprintf(toSend, "%010u:%06i:%03.3f:%03.3f:%03.3f:%03.3f:%03.3f:%03.3f:%05i:%05i:%05i:%02.2f:%02.2f:%01.3f:%01i:%01i:%010u:%03.3f:%03.3f:%01i:%s:%01i:%03.3f\n", // about 97 bytes? (800-900 us)
  // 	micros(), 
	// totalRevolutions, 
	// m_ect->getReading(), 
	// m_iat->getReading(), 
	// m_map->getReading(), 
	// m_map_avg->getSensorAvg(),
	// m_tps->getReading(), 
	// AFR, 
	// RPM, 
  // injectorPulseTime,
	// lastPulse,
	// scaledMAP, 
	// scaledRPM,
	// AFR,
	// true,
	// haveInjected,
	// m_map_avg->getTroughTime(),
  // m_map_avg->getPrevD(),
  // m_map_avg->getSensorGauss(),
  // SDConnected,
  // fileName,
  // m_ect->getReading() > MAX_ALLOWABLE_ECT,
  // AFR);

  if(SDConnected) { // open and write to file
    File logFile = SD.open(fileName, FILE_WRITE);
    logFile.write(ecuData);
    logFile.close();
  }
  
  //sendInfo(ecuData);
  //Serial.write(ecuData);
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
      m_haveInjected = false;
  }
}
