#include "OvenControl.h"
#include "LogData.h"
#include "TemperatureSensor.h"
#include "RealTimeClock.h"
#include "Defines.h"
#include "ProgramFiles.h"
#include "gcc-diagnostics.h"

//----------------------------------------------------------------------------------------------------------------------
//   INIT OVEN CONTROL
//----------------------------------------------------------------------------------------------------------------------

void initOvenControl (void) {
  pinMode (PIN_OVEN_RELAY, OUTPUT) ;
  pinMode (LED_EN_MARCHE, OUTPUT) ;
}

//----------------------------------------------------------------------------------------------------------------------
//  OVEN MODES
//----------------------------------------------------------------------------------------------------------------------

enum class OvenMode { stopped, manualMode, automaticMode } ;

//----------------------------------------------------------------------------------------------------------------------
//  OVEN VARIABLES
//----------------------------------------------------------------------------------------------------------------------

static double gTemperatureReference ;
static uint32_t gAutomaticStartDelayInSecondes ;
static uint32_t gRunningTime ; // In seconds
static OvenMode gOvenMode = OvenMode::stopped ;
static uint8_t gDelayForChangeCommand ;
static bool gCurrentOvenCommand ;

static const uint8_t DELAY_BETWEEN_COMMAND_CHANGE = 5 ; // In seconds

//----------------------------------------------------------------------------------------------------------------------
//   LOG
//----------------------------------------------------------------------------------------------------------------------

static LogData gLogData ;

//----------------------------------------------------------------------------------------------------------------------
//   RUNNING TIME (in seconds)
//----------------------------------------------------------------------------------------------------------------------

uint32_t ovenRunningTime (void) {
  return gRunningTime ;
}

//----------------------------------------------------------------------------------------------------------------------
//   START OVEN
//----------------------------------------------------------------------------------------------------------------------

static void configureOnStart (void) {
  gDelayForChangeCommand = 0 ;
  gCurrentOvenCommand = false ;
//--- Init log Data
  const RtcDateTime t = currentDateTime () ;
  snprintf (gLogData.mFileName, FILE_NAME_SIZE, "%u-%u-%u-%uh%umin%us",
            t.Day (),
            t.Month (),
            t.Year (),
            t.Hour (),
            t.Minute (),
            t.Second ()
  ) ;
  gLogData.mLogStartTime = 0 ;
  gLogData.mLogImageSize = 0 ;
}


//----------------------------------------------------------------------------------------------------------------------
//   START OVEN IN MANUAL MODE
//----------------------------------------------------------------------------------------------------------------------

void startOvenInManualMode (const uint32_t inTemperatureReference) { // Consigne in Celcius
  if (gOvenMode == OvenMode::stopped) {
    gTemperatureReference = (double) inTemperatureReference ;
    gRunningTime = 0 ;
    configureOnStart () ;
  //--- Start oven
    gOvenMode = OvenMode::manualMode ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
//   SET TEMPERATURE REFERENCE IN MANUAL MODE
//----------------------------------------------------------------------------------------------------------------------

void setTemperatureReferenceInManualMode (const int32_t inTemperatureReference) { // Consigne in Celcius
  if (gOvenMode == OvenMode::manualMode) {
    gTemperatureReference = (double) inTemperatureReference ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
//   START OVEN IN AUTOMATIC MODE
//----------------------------------------------------------------------------------------------------------------------

void startOvenInAutomaticMode (const uint32_t inDelayInMinutes) {
  if (gOvenMode == OvenMode::stopped) {
    gRunningTime = 0 ;
    gAutomaticStartDelayInSecondes = inDelayInMinutes * 60 ;
    gOvenMode = OvenMode::automaticMode ;
  }
}

//----------------------------------------------------------------------------------------------------------------------

uint32_t delayForStartingInSeconds (void) {
  return gAutomaticStartDelayInSecondes ;
}

//----------------------------------------------------------------------------------------------------------------------
//   STOP OVEN
//----------------------------------------------------------------------------------------------------------------------

void stopOven (void) {
  if (gOvenMode != OvenMode::stopped) {
    enterLogData (gLogData) ;
    gOvenMode = OvenMode::stopped ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
//   OVEN IS RUNNING
//----------------------------------------------------------------------------------------------------------------------

bool ovenIsRunning (void) {
  return gOvenMode != OvenMode::stopped ;
}

//----------------------------------------------------------------------------------------------------------------------
//  OVEN CONTROL CORE
//----------------------------------------------------------------------------------------------------------------------

static void ovenControlCore (const int32_t inSensorTemperature) {
  if (gDelayForChangeCommand == 0) {
    const bool ovenIsOn = inSensorTemperature < int32_t (gTemperatureReference) ;
    digitalWrite (PIN_OVEN_RELAY, ovenIsOn) ;
    if (gCurrentOvenCommand != ovenIsOn) {
      gCurrentOvenCommand = ovenIsOn ;
      gDelayForChangeCommand = DELAY_BETWEEN_COMMAND_CHANGE ;
    }
  }else{
    gDelayForChangeCommand -= 1 ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
//  LOG DATA
//----------------------------------------------------------------------------------------------------------------------

static void logData (const uint32_t inTemperature) {
  Record record ;
  record.mConsigne = lround (gTemperatureReference) ;
  record.mOvenIsOn = gCurrentOvenCommand ? (record.mConsigne + 10) : 0 ;
  record.mTemperature = inTemperature ;
  gLogData.mLogImage [gLogData.mLogImageSize] = record ;
  gLogData.mLogImageSize += 1 ;
//--- If log array is full, write to file
  if (gLogData.mLogImageSize == RECORD_SIZE) {
    enterLogData (gLogData) ;
    gLogData.mLogStartTime += gLogData.mLogImageSize ;
    gLogData.mLogImageSize = 0 ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
//  EXECUTE OVEN CONTROL
//----------------------------------------------------------------------------------------------------------------------

void runOvenControlFromISR (void) {
  switch (gOvenMode) {
  case OvenMode::stopped :
    digitalWrite (PIN_OVEN_RELAY, LOW) ;
    digitalWrite (LED_EN_MARCHE, LOW) ;
    break ;
  case OvenMode::manualMode :
    {
    //--- Blink running led
      digitalWrite (LED_EN_MARCHE, !digitalRead (LED_EN_MARCHE)) ;
    //--- Get Temperature
      const int32_t temperature = lround (getSensorTemperature ()) ;
    //--- Control
      ovenControlCore (temperature) ;
    //--- Record data for logging
      logData (temperature) ;
    //--- Increment running time
      gRunningTime += 1 ;
    }
    break ;
  case OvenMode::automaticMode :
  //--- Blink running led
    digitalWrite (LED_EN_MARCHE, !digitalRead (LED_EN_MARCHE)) ;
  //--- Delay or running ?
    if (gAutomaticStartDelayInSecondes > 0) { // Delay ?
      gAutomaticStartDelayInSecondes -= 1 ;
    }else{ // Running
      if (gRunningTime == 0) {
        configureOnStart () ;
      }
    //--- Update reference
      gTemperatureReference = programTemperatureReferenceForRunningTime (gRunningTime) ;
    //--- Get Temperature
     const int32_t temperature = lround (getSensorTemperature ()) ;
   //--- Control
      ovenControlCore (temperature) ;
   //--- Record data for logging
      logData (temperature) ;
    //--- Stop or increment running time ?
      if (gRunningTime < (programDurationInMinutes () * 60)) {
        gRunningTime += 1 ;
      }else{
        stopOven () ;
      }
    }
    break ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
//   TEMPERATURE REFERENCE
//----------------------------------------------------------------------------------------------------------------------

double temperatureReference (void) {
  return gTemperatureReference ;
}

//----------------------------------------------------------------------------------------------------------------------
