//----------------------------------------------------------------------------------------------------------------------
// Project by Antoine COLSON and Clément NADER
// directed by Pierre MOLINARO and Jean-Claude BARDIAUX
//
// This sketch is used to control an oven in order to follow a temperature graph 
//
//#####################################################################################################################
//################################################ A PROJECT FROM ECN  ################################################
//#####################################################################################################################
//
//----------------------------------------------------------------------------------------------------------------------
//  Check board type
//----------------------------------------------------------------------------------------------------------------------

#ifndef ARDUINO_MH_ET_LIVE_ESP32MINIKIT
  #error "Select 'MH ET LIVE ESP32MiniKit' board"
#endif

//----------------------------------------------------------------------------------------------------------------------
//  Libraries
//    - TFT_eSPI
//    - Rtc_by_Makuna
//----------------------------------------------------------------------------------------------------------------------

#include "Defines.h"
#include "MainMenu.h"
#include "RotaryEncoder.h"
#include "TFT.h"
#include "RealTimeClock.h"
#include "TemperatureSensor.h"
#include "SDCard.h"
#include "Backlight.h"
#include "ManualMode.h"
#include "OvenControl.h"
#include "LogData.h"

//----------------------------------------------------------------------------------------------------------------------
//                                    SETUP
//----------------------------------------------------------------------------------------------------------------------

void setup (void) {
// ----------DEBUGGING section of setup----------
  Serial.begin (115200) ;     // DEBUGGING: opens serial port, sets data rate to 115200 bps
// ----------LEDs section of setup----------
  pinMode (LED_HOT_OVEN, OUTPUT) ;
//--- Backlight
  initBacklight () ;
//--- Initializations
  initTemperatureSensor () ;
  initEncoder () ;
  initRealTimeClock () ;
  initOvenControl () ;
// ----------SD card reader section of setup----------
  initSDcard () ;
// ----------TFT screen section of setup----------
  initScreen () ;
}

//----------------------------------------------------------------------------------------------------------------------
//                                    LOOP
//----------------------------------------------------------------------------------------------------------------------

void loop (void) {
//--- Backlight
  updateBacklight () ;
//----------Updating the time----------
  updateTime () ;
// ----------Updating the temperature----------
  updateTemp () ;
  updateSDCardStatus () ; // Should be before writeLogFile
  writeLogFile () ;
// ----------Updating the state of the LEDs----------
// Light on the LED 1 if the oven is hot
  digitalWrite (LED_HOT_OVEN, getSensorTemperature () > 200.0) ;
//--- Handle user interface
  updateUserInterface () ;
}

//----------------------------------------------------------------------------------------------------------------------
