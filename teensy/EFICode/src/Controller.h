#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Constants.h"
#include "sensors/AnlgSensor.h"
#include "sensors/SensorAvg.h"
#include "sensors/IATSensor.h"

#include "modules/RevCounter.h"
#include "modules/Speedometer.h"
#include "modules/EFIHardware.h"
#include "modules/EngineStateArbitrator.h"
#include "modules/EFI.h"
#include "modules/AFRLoader.h"

#include "utils/spi_adc.h"

class Controller {
public:
  // Constructor
  Controller();

  // Updates sensor values.
  bool readSensors();

  // Sends the current sensor values and requested parameter information.
  void sendCurrentData();

  // If the currentlySendingData flag is set and timer1 triggers an interrupt sends out a data sample.
  void trySendingData();

  // Increments the counted number of revolutions since the last RPM update.
  // Every 2nd revolution, switches the INJ_Pin to HIGH and sets a timer for
  // the amount of time equal to the most recent calculated pulse time.
  void onRevDetection();

  // Updates the RPM reading by dividing the total number of revolutions since
  // the last update by the number of minutes that have passed.
  void updateRPM();

  // Performs 2D interpolation to lookup a pulse time for the given engine conditions.
  void lookupPulseTime();

  // Checks to see if the engine is on or off. If the engine switches state since the last
  // check, changes parameters accordingly.
  void updateEngineState();

  // Turns the injector on if it is not disabled.
  void pulseOn();

  // Turns the injector off.
  void pulseOff();

  // Attaches the interrupt timer for shutting the injector off and
  // sets the INJisDisabled flag to false.
  void enableINJ();

  // Detaches the interrupt timer for shutting the injector off and
  // sets the INJisDisabled flag to true.
  void disableINJ();

  void initializeParameters();

  bool sendInfo(char* str);

  void dumpBasePulseTimes() const;
  
private:
  // Has a value of true if the timer3 interrupt is detached from the "pulseOff" function.
  // Also prevents the injector from pulsing on if true.
  SPI_ADC* m_adc;

  bool refreshAvailable;
  const int* sensorVals;

  //Data Retrieval
  bool enableSendingData;
  bool currentlySendingData; // used to disable data transmission during injection
  bool SDConnected;
  const char baseFileName[6] = "sdlog";
  char fileName[20] = "NOFILE";

  AnlgSensor* m_ect;
  IATSensor* m_iat;
  AnlgSensor* m_map; //MAP module, responsible for collecting data and processing data from the Manifold Pressure Sensor
  AnlgSensor* m_tps; //TPS Module, responsible for collecting TPS
  
  SensorAvg* m_map_avg;

  AFRLoader* m_afrLoader;
  RevCounter* m_revCounter;
  Speedometer* m_speedometer;
  EngineStateArbitrator* m_esa;
  EFIHardware* m_efih;
  EFI* m_efi;

  bool m_haveInjected;
  //
  // For some undocumented reason they use this table to account for
  // Volumetric Efficiency so these values are more like AFR/VE
  //
  // double fuelRatioTable[numTableRows][numTableCols] =
  //   {
  //   {14.5,14.8,15.0,15.3,15.3,15.2,14.6,14.3,13.8,13.6},  // minimum pressure
  //   {14.5,14.8,15.0,15.3,15.3,15.2,14.6,14.3,13.8,13.6},
  //   {14.5,14.8,15.0,15.3,15.3,15.2,14.6,14.3,13.8,13.6},
  //   {14.5,14.8,15.5,15.3,16.0,16.0,15.7,14.3,13.8,13.6},
  //   {14.5,16.0,17.2,17.2,16.0,16.0,15.7,14.3,13.8,13.6},
  //   {14.5,17.0,17.2,17.2,16.0,16.0,15.7,14.3,13.8,13.6},
  //   {14.5,17.0,17.2,17.2,15.0,15.2,14.6,14.3,13.8,13.6},
  //   {14.5,14.8,17.2,17.2,15.0,15.2,14.6,14.3,13.8,13.6},
  //   {14.5,14.8,17.2,17.2,15.3,15.2,14.6,14.3,13.8,13.6},
  //   {14.5,14.8,15.2,15.3,15.3,15.2,14.6,14.3,13.8,13.6},
  //   {14.5,14.8,15.0,15.3,15.3,15.2,14.6,14.3,13.8,13.6}   // maximum pressure
  //   };


  double m_fuelRatioTable[numTableRows][numTableCols];
  long injectorBasePulseTimes[numTableRows][numTableCols];
};

#endif
