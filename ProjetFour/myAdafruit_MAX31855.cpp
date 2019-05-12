//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//  This file is a fork from original Adafruit library
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

/***************************************************
  This is a library for the Adafruit Thermocouple Sensor w/MAX31855K

  Designed specifically to work with the Adafruit Thermocouple Sensor
  ----> https://www.adafruit.com/products/269

  These displays use SPI to communicate, 3 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

#include "myAdafruit_MAX31855.h"

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// La lecture par (soft) SPI renvoie une valeur sur 32 bits dont la composition est la suivante :
//      D31 ... D18 : 14 bits signés donnant la température en Celcius
//      D17 : réservé, toujours lu à 0
//      D16 : 1 --> faute, 0 --> ok
//      D15 ... D4 : 12 bits donnant la température interne (inutilisés)
//      D3 : réservé, toujours lu à 0
//      D2 : 0 --> ok, 1 --> SCV Fault : « the thermocouple is short-circuited to VCC »
//      D1 : 0 --> ok, 1 --> SCG Fault : « the thermocouple is short-circuited to GND »
//      D0 : 0 --> ok, 1 -->  OC Fault : « the thermocouple is open (no connections) »
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   Constructor
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

myAdafruit_MAX31855::myAdafruit_MAX31855 (const uint8_t inSCLK, const uint8_t inCS, const uint8_t inMISO) :
mSCLK (inSCLK),
mCS (inCS),
mMISO (inMISO) {
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   Configuration
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void myAdafruit_MAX31855::begin(void) {
//--- Define pin modes
  pinMode (mCS, OUTPUT) ;
  digitalWrite (mCS, HIGH) ;
  pinMode (mSCLK, OUTPUT) ;
  pinMode (mMISO, INPUT) ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   Get raw value
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

uint32_t myAdafruit_MAX31855::rawValue(void) {
  uint32_t result = 0 ;

  digitalWrite (mCS, LOW) ;
  delay (1) ;

  digitalWrite (mSCLK, LOW) ;
  delay (1) ;

  for (int32_t i = 31 ; i >= 0 ; i--) {
    digitalWrite (mSCLK, LOW);
    delay (1) ;
    result <<= 1 ;
    if (digitalRead (mMISO)) {
	    result |= 1 ;
    }

    digitalWrite (mSCLK, HIGH) ;
    delay (1) ;
  }

  digitalWrite (mCS, HIGH) ;
  return result ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
