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
//  Check board type (Teensy 3.5)
//----------------------------------------------------------------------------------------------------------------------

#include "check-board.h"

//----------------------------------------------------------------------------------------------------------------------
//  Libraries
//    - Installer manuellement ILI9341_t3n et SPIN
//    - Rtc_by_Makuna
//    - SdFat (Bill Greiman) 
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
#include "TimerInterrupt.h"

//----------------------------------------------------------------------------------------------------------------------
//                                    SETUP
//----------------------------------------------------------------------------------------------------------------------

void setup (void) {
// ----------DEBUGGING section of setup----------
  Serial.begin (9600) ;
  Serial.println ("Hello monitor!") ;
// ----------LEDs section of setup----------
  pinMode (LED_HOT_OVEN, OUTPUT) ;
  pinMode (LED_BUILTIN, OUTPUT) ;
  digitalWrite (LED_BUILTIN, HIGH) ;
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
  configureTimerInterrupt () ;
}

//----------------------------------------------------------------------------------------------------------------------
//                                    LOOP
//----------------------------------------------------------------------------------------------------------------------

static uint32_t gBlinkDate = 3000 ;

void loop (void) {
  if (gBlinkDate < millis ()) {
    digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
    gBlinkDate += 1000 ;
  }
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
