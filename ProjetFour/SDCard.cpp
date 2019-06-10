#include "SDCard.h"
#include "Backlight.h"

//----------------------------------------------------------------------------------------------------------------------
//   INIT
//----------------------------------------------------------------------------------------------------------------------

void initSDcard (void) {
  pinMode (SDCARD_CD, INPUT) ;
  updateSDCardStatus () ;
}

//----------------------------------------------------------------------------------------------------------------------
//   STATIC VARIABLES
//----------------------------------------------------------------------------------------------------------------------

static SPIClass hspi (HSPI) ;
static uint32_t gMountIndex ;
static SDCardStatus gSDCardStatus = SDCardStatus::noCard ; // Records current SD Card status

//--------------------------------------------------------------------------------------------------------

void updateSDCardStatus (void) {
  const bool inserted = digitalRead (SDCARD_CD) == HIGH ;
  switch (gSDCardStatus) {
  case SDCardStatus::noCard :
    if (inserted) {
      extendBackLightDuration () ;
      const bool mounted = SD.begin (SDCARD_CS, hspi) ; // SS spin, SPIClass -> HSPI
      if (mounted) {
        gSDCardStatus = SDCardStatus::mounted ;
        gMountIndex += 1 ;
      }else{
        SD.end () ;
        gSDCardStatus = SDCardStatus::insertedNotMounted ;
      }
    }
    break ;
  case SDCardStatus::mounted :
    if (!inserted) {
      extendBackLightDuration () ;
      SD.end () ;
      gSDCardStatus = SDCardStatus::noCard ;
    }
    break ;
  case SDCardStatus::insertedNotMounted :
    if (inserted) {
      const bool mounted = SD.begin (SDCARD_CS, hspi) ; // SS spin, SPIClass -> HSPI
      if (mounted) {
        extendBackLightDuration () ;
        gSDCardStatus = SDCardStatus::mounted ;
        gMountIndex += 1 ;
      }else{
        SD.end () ;
      }
    }else{
      extendBackLightDuration () ;
      gSDCardStatus = SDCardStatus::noCard ;
    }
    break ;
  }
}

//--------------------------------------------------------------------------------------------------------

SDCardStatus sdCardStatus (void) {
  return gSDCardStatus ;
}

//--------------------------------------------------------------------------------------------------------

uint32_t mountIndex (void) { // Incremented each time a card is mounted
  return gMountIndex ;
}

//--------------------------------------------------------------------------------------------------------

bool directoryExists (const String & inPath) {
  bool result = false ;
  File f = SD.open (inPath) ;
  if (f) {
    result = f.isDirectory () ;
  }
  f.close () ;
  return result ;  
}

//--------------------------------------------------------------------------------------------------------

bool createDirectory (const String & inPath) {
  return SD.mkdir (inPath) ;
}

//--------------------------------------------------------------------------------------------------------

File openFileForCreation (const String & inFilePath) {
  return SD.open (inFilePath, FILE_WRITE) ;
}

//--------------------------------------------------------------------------------------------------------

File openFileForAppending (const String & inFilePath) {
  return SD.open (inFilePath, FILE_APPEND) ;
}

//--------------------------------------------------------------------------------------------------------
