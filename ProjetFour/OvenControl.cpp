#include "OvenControl.h"
#include "TemperatureSensor.h"
#include "Defines.h"
#include "SD_Card.h"

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   FORWARD DECLARATIONS
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static void executeOvenControl (void) ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static SemaphoreHandle_t gSemaphore (xSemaphoreCreateCounting (10, 0)) ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   RUN OVEN CONTROL
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void runOvenControl (void) {
  xSemaphoreGive (gSemaphore) ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//  OVEN CONTROL TASK
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static void OvenControlTask (void * pData) {
  while (1) {
    xSemaphoreTake (gSemaphore, portMAX_DELAY) ;
    executeOvenControl () ;
  }
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   INIT OVEN CONTROL
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void initOvenControl (void) {
  pinMode (PIN_OVEN_RELAY, OUTPUT) ;
  pinMode (LED_EN_MARCHE, OUTPUT) ;
  xTaskCreate (OvenControlTask, "OvenControlTask", 1024, NULL, 256, NULL) ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//  OVEN MODES
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

enum class OvenMode { stopped, manualMode, automaticMode} ;
 
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//  OVEN VARIABLES
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static uint32_t gConsigne ;
static uint32_t gRunningTime ; // In seconds
static const uint32_t FILE_NAME_SIZE = 24 ;
static char gFileName [FILE_NAME_SIZE] ;
static OvenMode gOvenMode = OvenMode::stopped ;
static uint8_t gDelayForChangeCommand ;
static bool gCurrentOvenCommand ;

static const uint8_t DELAY_BETWEEN_COMMAND_CHANGE = 5 ; // In seconds

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   LOG
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

typedef struct {
  int mConsigne : 15 ;
  int mOvenIsOn :  1 ;
  int mTemperature : 15 ;
} Record ;

static const uint32_t RECORD_SIZE = 3600 ;
static Record gRecording [RECORD_SIZE] ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   IMAGE OF LOG
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static uint32_t gImageSize ;
static Record gRecordImage [RECORD_SIZE] ;
static char gFileNameImage [FILE_NAME_SIZE] ;
static bool gWriteLogFile ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   START OVEN
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void startOvenInManualMode (const uint32_t inConsigne, const RtcDateTime & inStartDateTime) { // Consigne in Celcius
  if (gOvenMode == OvenMode::stopped) {
    gConsigne = inConsigne ;
    gRunningTime = 0 ;
    gDelayForChangeCommand = 0 ;
    gCurrentOvenCommand = false ;
//--- File name
    snprintf (gFileName, FILE_NAME_SIZE, "%u-%u-%u-%uh%umin%us",
      inStartDateTime.Day (),
      inStartDateTime.Month (),
      inStartDateTime.Year (),
      inStartDateTime.Hour (),
      inStartDateTime.Minute (),
      inStartDateTime.Second ()
    ) ;
    gOvenMode = OvenMode::manualMode ;
  }
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   SET CONSIGNE IN MANUAL MODE
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void setConsigneInManualMode (const uint32_t inConsigne) { // Consigne in Celcius
  if (gOvenMode == OvenMode::manualMode) {
    gConsigne = inConsigne ;
  }
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   STOP OVEN
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void stopOven (void) {
  if (gOvenMode != OvenMode::stopped) {
    if (!gWriteLogFile) {
      gImageSize = gRunningTime ;
      for (uint32_t i=0 ; i < gImageSize ; i+=1) {
        gRecordImage [i] = gRecording [i] ;
      }
      strcpy (gFileNameImage, gFileName) ;
      gWriteLogFile = true ;
    }
    gOvenMode = OvenMode::stopped ;
  }
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   OVEN IS RUNNING
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

bool ovenIsRunning (void) {
  return gOvenMode != OvenMode::stopped ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//  EXECUTE OVEN CONTROL
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static void executeOvenControl (void) {
//  Serial.print ("Oven at ") ; Serial.println (millis ()) ;
  if (gOvenMode == OvenMode::stopped) {
    digitalWrite (PIN_OVEN_RELAY, LOW) ;
    digitalWrite (LED_EN_MARCHE, LOW) ;
  }else{
  //--- Blink running led
    digitalWrite (LED_EN_MARCHE, !digitalRead (LED_EN_MARCHE)) ;
  //--- Get Temperature
    const int32_t temperature = lround (getTemp ()) ;
  //--- Control
    if (gDelayForChangeCommand == 0) {
      const bool ovenIsOn = temperature < int32_t (gConsigne) ;
      digitalWrite (PIN_OVEN_RELAY, ovenIsOn) ;
      if (gCurrentOvenCommand != ovenIsOn) {
        gCurrentOvenCommand = ovenIsOn ;
        gDelayForChangeCommand = DELAY_BETWEEN_COMMAND_CHANGE ;
      }
    }else{
      gDelayForChangeCommand -= 1 ;
    }
  //--- Record
    if (gRunningTime < RECORD_SIZE) {
      Record record ;
      record.mConsigne = gConsigne ;
      record.mOvenIsOn = gCurrentOvenCommand ;
      record.mTemperature = temperature ;
      gRecording [gRunningTime] = record ;
    }
  //--- Increment running time
    gRunningTime += 1 ;
  }
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//  WRITE LOG FILE
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static const char * RESULT_DIRECTORY = "/resultats" ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void writeLogFile (void) {
  if (gWriteLogFile) {
  //--- Message
    Serial.print ("Result file : ") ; Serial.println (gFileNameImage) ;
  //--- Check if result directory exists
    const bool resultDirectoryExists = directoryExists (RESULT_DIRECTORY) ;
    Serial.print ("Result directory exists: ") ; Serial.println (resultDirectoryExists ? "yes" : "no") ;
    bool ok = resultDirectoryExists ;
  //--- Create result directory
    if (! resultDirectoryExists) {
      ok = createDirectory (RESULT_DIRECTORY) ;
      Serial.print ("Result directory creation: ") ; Serial.println (ok ? "yes" : "no") ;
    }
  //--- Append data to file (or create file if does not exist)
    File f ;
    if (ok) {
    //--- Build file path
      String filePath (RESULT_DIRECTORY) ;
      filePath += "/" ;
      filePath += gFileNameImage ;
      filePath += ".txt" ;
      f = openFileForAppending (filePath) ;
      ok = f ;
      Serial.print ("Open file for appending: ") ; Serial.println (ok ? "yes" : "no") ;
      if (!ok) { // Create file
        f = openFileForCreation (filePath) ;
        ok = f ;
        Serial.print ("Open file for creation: ") ; Serial.println (ok ? "yes" : "no") ;
      }
    }
  //--- Append data to file
    if (ok) {
      for (uint32_t i = 0 ; (i < gImageSize) && ok ; i++) {
        const Record record = gRecordImage [i] ;
        String line (i) ;
        line += ";" ;
        line += String (record.mConsigne) ;
        line += ";" ;
        line += String (record.mOvenIsOn) ;
        line += ";" ;
        line += String (record.mTemperature) ;
        ok = f.println (line) ;
      }
      Serial.print ("File appending: ") ; Serial.println (ok ? "yes" : "no") ;          
    }
  //--- Close file
    f.close () ;
  //---
    gWriteLogFile = false ;
  }
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
