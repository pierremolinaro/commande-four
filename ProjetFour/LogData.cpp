#include "LogData.h"
#include "Defines.h"
#include "SDCard.h"

//----------------------------------------------------------------------------------------------------------------------
//   STATIC VARIABLE
//----------------------------------------------------------------------------------------------------------------------

static bool gWriteLogFile ;
static LogData gLogData ;

//----------------------------------------------------------------------------------------------------------------------
//   ENTER LOG DATA (done by OvenControl thread)
//----------------------------------------------------------------------------------------------------------------------

void enterLogData (const LogData & inData) {
  if (!gWriteLogFile) {
    gLogData = inData ;
    gWriteLogFile = true ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
//   WRITE LOG FILE (done in main thread)
//----------------------------------------------------------------------------------------------------------------------

static const char * RESULT_DIRECTORY = "/resultats" ;


void writeLogFile (void) {
  if (gWriteLogFile && (sdCardStatus () == SDCardStatus::mounted)) {
  //--- Message
    Serial.print ("Result file : ") ; Serial.println (gLogData.mFileName) ;
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
      filePath += gLogData.mFileName ;
      filePath += ".csv" ;
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
      for (uint32_t i = 0 ; (i < gLogData.mLogImageSize) && ok ; i++) {
        const Record record = gLogData.mLogImage [i] ;
        String line (gLogData.mLogStartTime + i) ;
        line += ";" ;
        line += String (record.mConsigne) ;
        line += ";" ;
        line += String (record.mOvenIsOn) ;
        line += ";" ;
        line += String (record.mTemperature) ;
        line += "\n" ;
        ok = f.print (line) ;
      }
      Serial.print ("File appending: ") ; Serial.println (ok ? "yes" : "no") ;          
    }
  //--- Close file
    f.close () ;
  //---
    gWriteLogFile = false ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
