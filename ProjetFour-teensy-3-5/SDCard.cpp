#include "SDCard.h"
#include "Backlight.h"
#include "gcc-diagnostics.h"

//----------------------------------------------------------------------------------------------------------------------

SdFat SD ;

//----------------------------------------------------------------------------------------------------------------------
//   INIT
//----------------------------------------------------------------------------------------------------------------------

void initSDcard (void) {
  pinMode (SDCARD_CD, INPUT) ;
  SPI.setSCK (SDCARD_SCK) ;
  updateSDCardStatus () ;
}

//----------------------------------------------------------------------------------------------------------------------
//   STATIC VARIABLES
//----------------------------------------------------------------------------------------------------------------------

static uint32_t gMountIndex ;
static SDCardStatus gSDCardStatus = SDCardStatus::noCard ; // Records current SD Card status

//--------------------------------------------------------------------------------------------------------

void updateSDCardStatus (void) {
  const bool inserted = digitalRead (SDCARD_CD) == HIGH ;
  switch (gSDCardStatus) {
  case SDCardStatus::noCard :
    if (inserted) {
      extendBackLightDuration () ;
      const bool mounted = SD.cardBegin (SDCARD_CS, SD_SCK_MHZ (25)) ;
      if (mounted) {
        gSDCardStatus = SDCardStatus::mounted ;
        gMountIndex += 1 ;
      }else{
        // SD.cardEnd () ; §
        gSDCardStatus = SDCardStatus::insertedNotMounted ;
      }
    }
    break ;
  case SDCardStatus::mounted :
    if (!inserted) {
      extendBackLightDuration () ;
      // SD.end () ; §
      gSDCardStatus = SDCardStatus::noCard ;
    }
    break ;
  case SDCardStatus::insertedNotMounted :
    if (inserted) {
      const bool mounted = SD.cardBegin (SDCARD_CS, SD_SCK_MHZ (25)) ;
      if (mounted) {
        extendBackLightDuration () ;
        gSDCardStatus = SDCardStatus::mounted ;
        gMountIndex += 1 ;
      }else{
        // SD.end () ; §
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
  return SD.mkdir (inPath.c_str ()) ;
}

//--------------------------------------------------------------------------------------------------------

File openFileForCreation (const String & inFilePath) {
  return SD.open (inFilePath.c_str (), O_WRITE) ;
}

//--------------------------------------------------------------------------------------------------------

File openFileForAppending (const String & inFilePath) {
  return SD.open (inFilePath.c_str (), O_APPEND) ;
}

//--------------------------------------------------------------------------------------------------------
