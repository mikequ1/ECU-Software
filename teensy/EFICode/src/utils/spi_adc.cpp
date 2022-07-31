#include "spi_adc.h"

#include "Arduino.h"
#include "SPI.h"

volatile bool SPI_ADC::validVals;
volatile bool SPI_ADC::fetchVals;

// SPI Settings
SPISettings SPI_PARAMS(10000000, MSBFIRST, SPI_MODE0); // 1 MHz, MODE0

SPI_ADC::SPI_ADC() {

  // Conversion Start Pin
  pinMode(ADC_nCNVST_PIN, OUTPUT);
  digitalWrite(ADC_nCNVST_PIN, HIGH);

  // End-of-Conversion Pin, Attach Interrupt
  pinMode(ADC_nEOC_PIN, INPUT);
  //attachInterrupt(digitalPinToInterrupt(ADC_nEOC_PIN), dummy, FALLING);
  //detachInterrupt(digitalPinToInterrupt(ADC_nEOC_PIN));
  //attachInterrupt(digitalPinToInterrupt(ADC_nEOC_PIN), handle_EOC, FALLING);

  // Setup SPI
  pinMode(ADC_nCS_PIN, OUTPUT);
  digitalWrite(ADC_nCS_PIN, HIGH);

  SPI.begin();

  // MAX11624  Setup
  SPI.beginTransaction(SPI_PARAMS);

    // Select ADC
  digitalWrite(ADC_nCS_PIN, LOW);
    // Write to conversion, setup, and avg registers
  SPI.transfer(MAX11624_CONV_REG);
  SPI.transfer(MAX11624_SETUP_REG);
  SPI.transfer(MAX11624_AVG_REG);
    // De-Select ADC
  digitalWrite(ADC_nCS_PIN, HIGH);

  SPI.endTransaction();
}


void SPI_ADC::refresh() {
  // Get Conversion Time
  convTime = micros();

  // Pull CNVST low for atleast 40 ns to start conversion
  digitalWrite(ADC_nCNVST_PIN, LOW);
  unsigned long temp = micros();
  while( micros() < temp + 1); // waiting 100 ns (CHANGE TO TIMER?)
  digitalWrite(ADC_nCNVST_PIN, HIGH);
  SPI_ADC::validVals = false;
}

const int * SPI_ADC::getChannels() {
  if (SPI_ADC::validVals) {
    if (SPI_ADC::fetchVals) {
        // Read from ADC's FIFO
      int data;
      for (int i = 0; i < ADC_CHANNELS; i++) {
        data = 0;
        
        SPI.beginTransaction(SPI_PARAMS);     // Get and store values from ADC FIFO
        digitalWrite(ADC_nCS_PIN, LOW);       // Select ADC
        data |= (int(SPI.transfer(0)) << 6);  // MSBs first
        digitalWrite(ADC_nCS_PIN, HIGH);      // De-Select ADC
        SPI.endTransaction();

        unsigned long temp = micros();
        while(micros() < temp + 5);

        SPI.beginTransaction(SPI_PARAMS);     // Get and store values from ADC FIFO
        digitalWrite(ADC_nCS_PIN, LOW);       // Select ADC
        data |= int(SPI.transfer(0) >> 2);         // LSBs next
        digitalWrite(ADC_nCS_PIN, HIGH);       // De-Select ADC
        SPI.endTransaction();

        valChannel[i] = data;
      }

      SPI_ADC::fetchVals = false;
    }

    return valChannel;
  }

  return ERRORCHANNELS; // obvious error (hopefully)
}

void SPI_ADC::checkEOC() {
  if (!digitalRead(ADC_nEOC_PIN))
    handle_EOC();
}

static void SPI_ADC::handle_EOC() {          // EOC interrupt
  SPI_ADC::validVals = true;
  SPI_ADC::fetchVals = true;
}

  
static void SPI_ADC::dummy() {;}             // dummy for interrupt
