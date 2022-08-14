// Author: Arhison

// MAX11624 SPI_ADC Class:
//  NOTE: Since static and volatile functions/variables are used here, only 1 ADC should be instantiated in a given program
//  Description:
//  - refreshVals()
//    - update all sensor values
//      1) tell the adc to start collecting data, make sensor values invalid
//      2) on interrupt, the values will be grabbed
//  - validVals()
//    - check if the values available are valid (since refreshVals was last called)
//  - getChannel(int n)
//    - get most recent value from Channel N of the ADC

// MAX11624 Theory of Operation:
//  Here we use the Clock Mode option 00. This lets us use CNVST to start a 
//    conversion for all channels (done by calling refresh()). Then we wait 
//    for EOC (end-of-conversion) (captured by interrupt) and read from the adc.
//    Use validVals() to learn when EOC has arrived and new values are 
//    available.

#ifndef SPI_ADC_H
#define SPI_ADC_H

#include "SPI.h"

// ADC SPI Communication Pins
const int ADC_nCNVST_PIN = 9;
const int ADC_nEOC_PIN   = 29;
const int ADC_nCS_PIN    = 10;
const int SPI_SCLK_PIN   = 27;
const int SPI_MOSI_PIN   = 11;
const int SPI_MISO_PIN   = 12;

// MAX11624 Register Values:
const char MAX11624_CONV_REG  = (0b1<<7)|(0b1111<<3)|(0b00<<0);  
                                // (CONV_REG)|(CHAN N = 15)|(SCAN THROUGH CHAN N)
const char MAX11624_SETUP_REG = (0b01<<6)|(0b00<<4)|(0b01<<2)|(0b00<<0);  
                                // (SETUP_REG)|(CLOCK MODE 00)|(EXT REF)|(D.C)
const char MAX11624_AVG_REG   = (0b001<<5)|(0b1<<4)|(0b00<<2)|(0b00<<0);  
                                // (AVG_REG)|(ENABLE AVG)|(AVG 4X)|(D.C)

// Other Settings
const int ADC_CHANNELS = 16;

class SPI_ADC {
public:
  // Constructor
  SPI_ADC();

  // Refresh all ADC values
  void refresh();

  // See if values are valid
  inline int get_validVals() {
    return validVals;
  }

  // Getter for value at channel n
  const int * getChannels();
  
  inline int getConvTime() {
    return convTime;
  }
  static void checkEOC();

private:  
  volatile static bool validVals;
  volatile static bool fetchVals;
  
  static void handle_EOC();           // EOC interrupt
  static void dummy();                // dummy for interrupt

  unsigned long convTime;             // Time of the previous conversion
  int valChannel[ADC_CHANNELS];
  const int ERRORCHANNELS[ADC_CHANNELS] = { -1, -1, -1, -1, -1, -1, -1, -1, 
                            -1, -1, -1, -1, -1, -1, -1};
};

#endif
