# EFI 2022-2023 V1 Documentation

prepared by mike qu

# EFICode.ino

## Main Goals of our ECU
* Electronic Fuel Injection: Robustness, Efficiency, Reliability
* Output useful data for analysis
<br>

## A list of functions called by interrupts
* `dummy()`
* `countRev()` -> `c->countRevolution();`
* `handle_pulseTimerTimeout()` -> `c->pulseOff();`
* `handle_sendData()` -> `c->trySendingData();`


# Controller Module 


## Controller.cpp
<br>

***

### Constructor

>Returns: None\
>Parameters: None

Sets up an EFI Controller object 

```c++
Controller::Controller() {
    // Sets injector pin to output mode. All other pins default to input mode.
    pinMode(INJ_Pin, OUTPUT);

    // Initializes Serial input and output at the specified baud rate.
    Serial.begin(BAUD_RATE);

    // Prevent blocking caused by the lack of a serial connection with a laptop (fixed W22)
    long t = micros();
    while(!Serial && (micros() - t < 1e6));

    // Initializing message
    Serial.write("Initializing...\n");

    // Initialize parameters with their starting values.
    this->initializeParameters();

    // Update sensors to their initial values.
    this->readSensors();
}
```

***

### initializeParameters()
>Returns: None\
>Parameters: None

Initializes EFI's core parameters with their starting values

```c++
void Controller::initializeParameters() {
    // Initialize all 3 Rev Counters.
    revolutions = 0; // Counts revs within the current RPM calc period
    totalRevolutions = 0; // Counts total revs since ECU was first turned on
    startingRevolutions = 0; // Counts revs from the last time the engine was running

    // Number of revolutions that must pass before recalculating RPM.
    previousRev = micros(); // timestamp of last flywheel HES hit

    // Initialize ADC's SPI comms with the Microcontroller
    adc = new SPI_ADC();
    refreshAvailable = true;

    // Initialize AFR values.
    AFR = 0; // unused

    // Initialize Analog Sensors
    s_temp = new SensorTemp(); //Temperature Sensors: ECT and IAT
    s_map = new SensorMAP(); //MAP
    s_tps = new SensorTPS(); //TPS

    // Initialize MAP and RPM indicies to zero.
    mapIndex = 0; //discrete MAP levels for AFR table
    rpmIndex = 0; //discrete RPM levels for AFR table

    // Initialize injector to disabled mode.
    // Used to detach the timer interrupt for pulsing off when the engine is not running.
    INJisDisabled = true;
    constModifier = 1.0; //injector pulse time modifier

    // Used to determine the amount of fuel used. (W22)
    lastPulse = 0; // timestamp of latest injector pulse
    totalPulseTime = 0; // accumulated injector on time
    totalFuelUsed = 0; // approximation of how much total fuel is used

    enableSendingData = true;

    currentlySendingData = enableSendingData;
    haveInjected = false;
    if(enableSendingData) {
      SDConnected = SD.begin(BUILTIN_SDCARD); // check if SD card is connected
      if(SDConnected) { // find new fileName
        int fileNumber = 0;
        do {
          sprintf(fileName, "%s%i", baseFileName, fileNumber);
          fileNumber++;
        } while(SD.exists(fileName));
      }
    }

    // Calculate base pulse times from fuel ratio table. Should actually
    // store the last table used and recall it from memory here!
    calculateBasePulseTime(false, 0, 0);

    lastRPMCalcTime = micros();
}
```
***

### readSensors()

>Returns: Boolean, indicating whether read is successful (not implemented)\
>Parameters: None

Retrieves analog sensor data readings from the MAX11624EEG+ ADC's outputs

For more information, check out the ADC section of the documentation

```c++
bool Controller::readSensors() {
  //If ADC can be refreshed, refresh ADC data
  if (refreshAvailable){
    adc->refresh();
    refreshAvailable = false;
  }

  adc->checkEOC();
  
  // If ADC values are valid, retrieve all analog sensor readings
  if (adc->get_validVals() == 1){
    const int* channels = adc->getChannels();
    sensorVals = channels;

    s_tps->getTPSSensor(sensorVals);
    s_temp->getECTSensor(sensorVals);
    s_temp->getIATSensor(sensorVals);
    s_map->readMAP(sensorVals);

    refreshAvailable = true;
  }
  return true;
}
```
***

### countRevolutions()

>Returns: None\
>Parameters: None

Records and increments the total amount of revolutions the engine has gone through. This function is also responsible for turning on the fuel injector on each of the engine's intake stroke. 

This function is triggered on an interrupt by the flywheel's Hall Effect Sensor

```c++
void Controller::countRevolution() {
  //  When called too soon, we skip countRevolution
  //  When micros() overflows, we continue as if its a normal countRevolution
  if (micros() - previousRev > 0 && (micros() - previousRev < minDelayPerRev))
    return;
  previousRev = micros();
  if (INJisDisabled) {
    enableINJ();
  }

  // Increment the number of revolutions
  revolutions++;
  totalRevolutions++;
  startingRevolutions++;

  // MAX TEMP CHECK
  if (s_temp->getECT() > MAX_ALLOWABLE_ECT){
    digitalWrite(LED_1, HIGH);
    return;
  }

  //Inject on every second revolution because this is a 4 stroke engine
  if (!detectEngineOff() && inStartingRevs()) {
    if (totalRevolutions % 2 == 1)
      pulseOn();
  } 
  else {  // inject when the time since the last trough is < 1 period (2 rotations between troughs)
    if (!detectEngineOff() && (s_map->getMapGauss() > s_map->getMap()))//&& ((60 * 1E6) / RPM > micros() - MAPTrough))
      pulseOn();
  }
}
```

* We keep three variables for counting engine revolutions
    * `revolutions` responsible for RPM calculations. It resets to 0 after each calculation cycle for RPM (see `updateRPM()`)
    * `totalRevolutions` counts the total amount of revs since the vehicle's electrical system was first turned on
    * `startingRevolutions` is responsible for measuring revolutions every time the engine restarts while the electrical system is still on. This works in tandem with the `inStartingRevs()` function to enable special fuel injection behavior when the engine is first turned on (more information below)            
* First, three conditions to eliminate where there may be undefined behavior
    * If called too soon since the last revolution (an unreasonably short amount of time (smaller than `minDelayPerRev`) has passed since the last hall effect sensor hit), the function returns without the rev counts incrementing
    * When `micros()` overflows (in which case `micros` will go back to 0 and become smaller than `previousRev`), the function returns with rev counts incrementing
    * If the engine is overheating (exceeds `MAX_ALLOWABLE_ECT`), the function also returns, preventing fuel combustion any further to protect mechanical components of the engine.
* If none of the above conditions occur, we proceed by recording the timestamp of the current HES hit, and increment all the rev counters
    * stored as variable `previousRev` so that it can be referred to in the future for RPM calculations
* There are two situations that calls for different ways to handle fuel injection
    * When the engine has just started, we inject fuel every two revolutions. This is because the intake stroke takes place once every two revolutions, for more information, look at the four-stroke engine (intake, compression, combustion, exhaust). 
    * After the engine has been running a while, we switch to use our MAP (manifold air pressure) sensor for fuel injection. Injection takes place when the average MAP is higher than the current instantaneous MAP, in other words: when there is a sudden dip in manifold pressure. This is because on an intake stroke, the cylinder's volume increases as the piston moves down, drawing air through the intake into the engine. 
    * Why do we treat the two differently
        * When the engine has just started, it is far from reaching its ideal working conditions yet in terms of temperature, pressure, fuel vaporization, etc. By manually timing it to inject every two revolutions, we can ensure it is getting fuel when needed.
        * After a while, the engine has reached its working temperatures, meaning idling and running will be a lot more consistent. In this case, relying on the pressure sensor would be more accurate as the hall effect sensor can sometimes deliver faulty readings (double-counts) over the long term. 
***

### doubleMap()

>Returns: double: mapped value\
>Parameters: doubles: current value, input range (lo, hi), output range (lo, hi)

Re-maps a floating-point double value from one range to another, analogous to the built-in Arduino function `map`

```c++
double Controller::doubleMap(double val, double minIn, double maxIn, double minOut, double maxOut) {
    return ((val - minIn) / (maxIn - minIn)) * (maxOut - minOut) + minOut;
}
```
* Mainly used to map data (particularly MAP and RPM) to a lower range where they can be cast into discrete values. This makes tuning for the Air-fuel ratio table much easier
***


### enableINJ()
>Returns: None\
>Parameters: None

Enables Fuel Injection (as the function name suggests)

```c++
void Controller::enableINJ() {
  INJisDisabled = false;
}
```
***

### disableINJ()
>Returns: None\
>Parameters: None

Disables Fuel Injection (As the function name suggests)

```c++
void Controller::disableINJ() {
  Timer3.stop();
  digitalWrite(INJ_Pin, LOW);
  noInterrupts();
  INJisDisabled = true;
  interrupts();
}
```
***

### pulseOn()
>Returns: None\
>Parameters: None

Turn on fuel injector

```c++
void Controller::pulseOn() {
  // disable data sending
  currentlySendingData = false;
  if (injectorPulseTime > 2.5E5)
    injectorPulseTime = 2.5E5;
  Timer3.setPeriod(injectorPulseTime);
  digitalWrite(INJ_Pin, HIGH);
  Timer3.start();
  noInterrupts(); //To ensure when lastPulse is used in pulseOff(), it isn't read as lastPulse is getting modified
  lastPulse = micros(); //Race Conditions Problem
  interrupts();
}
```
* During testing, there are cases in which the injector turns on, but never turns off again, spilling fuel everywhere through the throttle body. A temporary fix where we limit the maximum injector pulse time is applied but should be looked into further.
***

### pulseOff()
>Returns: None\
>Parameters: None

Turn off fuel injector

```c++
void Controller::pulseOff() {
  // When it's time to turn the injector off, follow these steps and turn it off
  digitalWrite(INJ_Pin, LOW);
  Timer3.stop();

  // Save the amount of time the injector pin spent HIGH.
  totalPulseTime += (micros() - lastPulse);

  // Let data be sent again
  currentlySendingData = enableSendingData;
  haveInjected = true;
}

```
***

### updateRPM()
>Returns: None\
>Parameters: None

Obtains a new RPM reading

```c++
void Controller::updateRPM() {
  //Prevent revolutions being read while it is being modified by the countRevolution() function associated with the interrupt
  noInterrupts();
  int tempRev = revolutions; 
  interrupts();

  if (tempRev >= revsPerCalc) {
    noInterrupts(); //To ensure that the interrupt of countRev doesn't get lost in case of bad timing of threads
    unsigned long currentRPMCalcTime = micros();
    if(currentRPMCalcTime - lastRPMCalcTime > 0) // only write if this value is positive (protect from overflow)
    	RPM = getRPM(currentRPMCalcTime - lastRPMCalcTime, tempRev); //Uses the previously determined value of revolutions to reduce
    //amount of noInterrupts() calls
    lastRPMCalcTime = currentRPMCalcTime;
    revolutions = 0; //Reset revolutions to 0 now that RPM count is complete (race condition modification)
    interrupts();

    // Potential project:
    // Should also dynamically change revsPerCalc. At lower RPM
    // the revsPerCalc should be lower but at higher RPM it should be higher.
  }
}
```
* Calculates RPM by measuring the time elapsed over revsPerCalc revs (hits of the flywheel HES)
***


### getFuelUsed()

>Returns: long (amount of fuel used since electrical startup in mL) \
>Parameters: None

Obtains the amount of fuel used since electrical system startup (in milliliters)

```c++
long Controller::getFuelUsed() {
  //volumetric flow rate = mass flow rate / density
  return givenFlow * totalPulseTime / density; //in mL
}
```
***


### interpolate2D()

>Returns: long (amount of fuel used since electrical startup in mL) \
>Parameters: int (bottom left row, col), double (unit x and y)

Performs interpolation to smooth out the AFR table, such that transitions from one range of RPM/MAP to another is gradual and not an instantaneous jump

```c++
long Controller::interpolate2D(int blrow, int blcol, double x, double y) {
    // Takes the coordinate of the bottom left corner of the square to perform 2D interpolation over.
    // x and y must be given in unit form. i.e., y = (yc-y1)/(y2-y1) and x = (xc-x1)/(x2-x1)
    // (0 <= y <= 1 and 0 <= x <= 1)
    return
    injectorBasePulseTimes[blrow][blcol]*(1-y)*(1-x)+
    injectorBasePulseTimes[blrow+1][blcol]*(y)*(1-x)+
    injectorBasePulseTimes[blrow][blcol+1]*(1-y)*(x)+
    injectorBasePulseTimes[blrow+1][blcol+1]*(y)*(x);
  }
```
***


### lookupPulseTime()

>Returns: None\
>Parameters: None

Figure out how much fuel to inject using the AFR table

```c++
void Controller::lookupPulseTime() {
    // Map the MAP and RPM readings to the dimensionns of the AFR lookup table
    noInterrupts();

    scaledMAP = doubleMap(s_map->getMapData(), minMAP, maxMAP, 0, numTableRows - 1); //number from 0 - numTableRows-1
    scaledRPM = doubleMap(RPM, minRPM, maxRPM, 0, numTableCols - 1); //number from 0 - numTableCols-1

    // Clip out of bounds to the min or max value, whichever is closer.
    scaledMAP = constrain(scaledMAP, 0, numTableRows - 1);
    scaledRPM = constrain(scaledRPM, 0, numTableCols - 1);

    // Get lower bounds for load and rpm indicies.
    mapIndex = scaledMAP; // double to int
    rpmIndex = scaledRPM;

    interrupts();

    // Clip extrapolation to the value at the max index. Otherwise, perform 2D interpolation to get
    // the base pulse time and then divide by the temperature.
    long tempPulseTime;
    if (rpmIndex < numTableCols - 1 && mapIndex < numTableRows - 1) {
        // Interpolation case
        tempPulseTime = interpolate2D(mapIndex, rpmIndex, scaledMAP-mapIndex, scaledRPM-rpmIndex) / s_temp->getIAT();
    }
    else {
        // Clipped case
        tempPulseTime = injectorBasePulseTimes[mapIndex][rpmIndex] / s_temp->getIAT();
    }

    // Add extra fuel for starting
    if (inStartingRevs()){
        tempPulseTime *= 1.4;
    }

    noInterrupts();
    injectorPulseTime = openTime + tempPulseTime * constModifier; // ADJUST OPEN TIME
    interrupts();
}
```
***


### calculateBasePulseTime()

>Returns: None\
>Parameters: None

Populate base injector pulse time table based on current pressure and other conditions

```c++
void Controller::calculateBasePulseTime(bool singleVal, int row, int col) {
  if (singleVal) {
    // Cover the range of pressure values from min - max inclusive.
    unsigned long pressure = map(row, 0, numTableRows - 1, minMAP, maxMAP);
    // Compute a base pulse time in units of microseconds * Kelvin. Temperature will be
    // divided on the fly to get the actual pulse time used.
    injectorBasePulseTimes[row][col] = 1E6 * pressure * injectionConstant / (fuelRatioTable[row][col]);
    return;
  }

  for (int x = 0; x < numTableRows; x++) {
    for (int y = 0; y < numTableCols; y++) {
      // Cover the range of pressure values from min - max inclusive.
      unsigned long pressure = map(x, 0, numTableRows - 1, minMAP, maxMAP);
      // Compute a base pulse time in units of microseconds * Kelvin. Temperature will be
      // divided on the fly to get the actual pulse time used.
      injectorBasePulseTimes[x][y] = 1E6 * pressure * injectionConstant / (fuelRatioTable[x][y]);
    }
  }
}
```




# Sensor Modules

## 