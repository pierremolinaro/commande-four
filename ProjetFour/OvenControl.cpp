#include "OvenControl.h"
#include "LogData.h"
#include "TemperatureSensor.h"
#include "Defines.h"

//----------------------------------------------------------------------------------------------------------------------
//   FORWARD DECLARATIONS
//----------------------------------------------------------------------------------------------------------------------

static void executeOvenControlTask (void) ;

//----------------------------------------------------------------------------------------------------------------------
//   SEMAPHORE
//----------------------------------------------------------------------------------------------------------------------

static SemaphoreHandle_t gSemaphore (xSemaphoreCreateCounting (10, 0)) ;

//----------------------------------------------------------------------------------------------------------------------
//   RUN OVEN CONTROL
//----------------------------------------------------------------------------------------------------------------------

void runOvenControlOnce (void) {
  xSemaphoreGive (gSemaphore) ;
}

//----------------------------------------------------------------------------------------------------------------------
//  OVEN CONTROL TASK
//----------------------------------------------------------------------------------------------------------------------

static void OvenControlTask (void * pData) {
  while (1) {
    xSemaphoreTake (gSemaphore, portMAX_DELAY) ;
    executeOvenControlTask () ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
//   INIT OVEN CONTROL
//----------------------------------------------------------------------------------------------------------------------

void initOvenControl (void) {
  pinMode (PIN_OVEN_RELAY, OUTPUT) ;
  pinMode (LED_EN_MARCHE, OUTPUT) ;
  xTaskCreate (OvenControlTask, "OvenControlTask", 1024, NULL, 256, NULL) ;
}

//----------------------------------------------------------------------------------------------------------------------
//  OVEN MODES
//----------------------------------------------------------------------------------------------------------------------

enum class OvenMode { stopped, manualMode, automaticMode } ;

//----------------------------------------------------------------------------------------------------------------------
//  OVEN VARIABLES
//----------------------------------------------------------------------------------------------------------------------

static uint32_t gTemperatureReference ;
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

void startOvenInManualMode (const uint32_t inTemperatureReference, const RtcDateTime & inStartDateTime) { // Consigne in Celcius
  if (gOvenMode == OvenMode::stopped) {
    gTemperatureReference = inTemperatureReference ;
    gRunningTime = 0 ;
    gDelayForChangeCommand = 0 ;
    gCurrentOvenCommand = false ;
  //--- Init log Data
    snprintf (gLogData.mFileName, FILE_NAME_SIZE, "%u-%u-%u-%uh%umin%us",
              inStartDateTime.Day (),
              inStartDateTime.Month (),
              inStartDateTime.Year (),
              inStartDateTime.Hour (),
              inStartDateTime.Minute (),
              inStartDateTime.Second ()
             ) ;
    gLogData.mLogStartTime = 0 ;
    gLogData.mLogImageSize = 0 ;
    //--- Start oven
    gOvenMode = OvenMode::manualMode ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
//   SET TEMPERATURE REFERENCE IN MANUAL MODE
//----------------------------------------------------------------------------------------------------------------------

void setTemperatureReferenceInManualMode (const int32_t inTemperatureReference) { // Consigne in Celcius
  if (gOvenMode == OvenMode::manualMode) {
    gTemperatureReference = inTemperatureReference ;
  }
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

static void ovenControlCore (const uint32_t inSensorTemperature) {
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
//  EXECUTE OVEN CONTROL
//----------------------------------------------------------------------------------------------------------------------

static void executeOvenControlTask (void) {
  //  Serial.print ("Oven at ") ; Serial.println (millis ()) ;
  if (gOvenMode == OvenMode::stopped) {
    digitalWrite (PIN_OVEN_RELAY, LOW) ;
    digitalWrite (LED_EN_MARCHE, LOW) ;
  }else{
  //--- Blink running led
    digitalWrite (LED_EN_MARCHE, !digitalRead (LED_EN_MARCHE)) ;
  //--- Get Temperature
    const int32_t temperature = lround (getSensorTemperature ()) ;
  //--- Control
    ovenControlCore (temperature) ;
  //--- Record data for logging
    Record record ;
    record.mConsigne = gTemperatureReference ;
    record.mOvenIsOn = gCurrentOvenCommand ;
    record.mTemperature = temperature ;
    gLogData.mLogImage [gLogData.mLogImageSize] = record ;
    gLogData.mLogImageSize += 1 ;
  //--- If log array is full, write to file
    if (gLogData.mLogImageSize == RECORD_SIZE) {
      enterLogData (gLogData) ;
      gLogData.mLogStartTime += gLogData.mLogImageSize ;
      gLogData.mLogImageSize = 0 ;
    }
  //--- Increment running time
    gRunningTime += 1 ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
