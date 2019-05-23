//----------------------------------------------------------------------------------------------------------------------
//  INCLUDES
//----------------------------------------------------------------------------------------------------------------------

#include "ProgramListMode.h"
#include "ProgramFiles.h"
#include "TFT.h"
#include "SDCard.h"
#include "RotaryEncoder.h"

//----------------------------------------------------------------------------------------------------------------------
//   CONSTANTS
//----------------------------------------------------------------------------------------------------------------------

static const uint32_t NAMES_IN_PAGES = 8 ;
static const char * const INSERT_SD_CARD_MESSAGE = "Ins" LOWERCASE_E_ACUTE "rer carte SD" ;
static uint32_t gDisplayOffset ;

//----------------------------------------------------------------------------------------------------------------------
//   STATIC VARIABLES
//----------------------------------------------------------------------------------------------------------------------

static uint32_t gSelectedItemIndex ;
static bool gSDCardIsMounted ;

enum class DisplayPhase { list, graph, table } ;

static DisplayPhase gDisplayPhase = DisplayPhase::list ;

//----------------------------------------------------------------------------------------------------------------------
//   STRING UTILITY
//----------------------------------------------------------------------------------------------------------------------

String stringWithSpaces (const uint32_t inCount) {
   String result ;
   for (uint32_t i=0 ; i<inCount ; i++) {
      result.concat (' ') ;
   }
   return result ;
}

//----------------------------------------------------------------------------------------------------------------------
//   ENTER PROGRAM LIST MODE
//----------------------------------------------------------------------------------------------------------------------

void enterProgramListMode (void) {
  tft.fillScreen (TFT_BLACK) ;
  gSDCardIsMounted = false ;
  gDisplayPhase = DisplayPhase::list ;
  gDisplayOffset = 0 ;
  gSelectedItemIndex = 0 ;
  setEncoderRange (0, gSelectedItemIndex, 0, true) ;
}

//----------------------------------------------------------------------------------------------------------------------
//   PRINT PROGRAM LIST MODE SCREEN
//----------------------------------------------------------------------------------------------------------------------

static void printProgramList (void) {
  tft.setTextSize (3) ;
  setLineForTextSize (0, 3) ;
  setMenuColor (gSelectedItemIndex == 0, false) ;
  tft.print (" Retour") ;
//--- Testing SD Card
  const bool sdCardIsMounted = sdCardStatus () == SDCardStatus::mounted ;
//--- Erase file names, or no card display ?
  if (gSDCardIsMounted && !sdCardIsMounted) {
    tft.setTextSize (2) ;
    tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
    const String pad = stringWithSpaces (FILE_BASE_NAME_MAX_LENGTH) ;
    for (uint32_t i = 0 ; i < (NAMES_IN_PAGES + 2) ; i++) {
      setLineForTextSize (2 + i, 2) ; setColumnForTextSize (1, 2) ;
      tft.print (pad) ;
    }
  }else if (sdCardIsMounted && !gSDCardIsMounted) {
    setLineForTextSize (2, 3) ; setColumnForTextSize (1, 3) ;
    tft.setTextSize (3) ;
    tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
    tft.print (stringWithSpaces (strlen (INSERT_SD_CARD_MESSAGE))) ;
  }
//---
  gSDCardIsMounted = sdCardIsMounted ;
  if (gSDCardIsMounted) {
    buildProgramFileNameArray () ;
    setLineForTextSize (2, 2) ; setColumnForTextSize (2, 2) ;
    tft.setTextSize (2) ;
    tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
  //--- Update offset
    if (programFileCount () <= NAMES_IN_PAGES) {
      gDisplayOffset = 0 ;
    }else{
      if (gSelectedItemIndex < NAMES_IN_PAGES / 2) {
        gDisplayOffset = 0 ;
      }else if (gSelectedItemIndex > (programFileCount () - NAMES_IN_PAGES / 2)) {
        gDisplayOffset = programFileCount () - NAMES_IN_PAGES ;
      }else{
        gDisplayOffset = gSelectedItemIndex - 1 - NAMES_IN_PAGES / 2 ;
      }
    }
    setLineForTextSize (2, 2) ; setColumnForTextSize (1, 2) ;
    tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
    tft.print ((gDisplayOffset == 0) ? "   " : "...") ;
    const uint32_t displayCount = min (programFileCount (), NAMES_IN_PAGES) ;
    for (uint32_t i = 0 ; i < displayCount ; i++) {
      setLineForTextSize (3 + i, 2) ; setColumnForTextSize (1, 2) ;
      setMenuColor (gSelectedItemIndex == (i + gDisplayOffset + 1), false) ;
      const String fileName = programFileNameAtIndex (i + gDisplayOffset) ;
      tft.print (fileName) ;
      tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
      uint32_t x = fileName.length () ;
      while (x < FILE_BASE_NAME_MAX_LENGTH) {
        x += 1 ;
        tft.print (" ") ;
      }
    }
    const bool displayFooter = (gDisplayOffset + displayCount) < programFileCount () ;
    setLineForTextSize (3 + NAMES_IN_PAGES, 2) ; setColumnForTextSize (1, 2) ;
    tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
    tft.print (displayFooter ? "..." : "   ") ;
    if (getEncoderMaxValue () != programFileCount ()) {
      setEncoderRange (0, gSelectedItemIndex, programFileCount (), true) ;
    }
  }else{
    setLineForTextSize (2, 3) ; setColumnForTextSize (1, 3) ;
    tft.setTextSize (3) ;
    tft.setTextColor (TFT_RED, TFT_BLACK) ;
    tft.print (INSERT_SD_CARD_MESSAGE) ;
    gSelectedItemIndex = 0 ;
    setEncoderRange (0, 0, 0, true) ;
  }
}

//----------------------------------------------------------------------------------------------------------------------

void printProgramListModeScreen (void) {
  switch (gDisplayPhase) {
  case DisplayPhase::list :
    printProgramList () ;
    break ;
  case DisplayPhase::graph :
    break ;
  case DisplayPhase::table :
    break ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
//   HANDLE ROTARY ENCODER IN PROGRAM LISTMODE
//----------------------------------------------------------------------------------------------------------------------

void handleRotaryEncoderInProgramListMode (void) {
  if (gDisplayPhase == DisplayPhase::list) {
    gSelectedItemIndex = getEncoderValue () ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
//   CLICK IN PROGRAM LIST MODE
//----------------------------------------------------------------------------------------------------------------------

void clickInProgramListMode (bool & outReturnToMainMenu) {
  if (gSelectedItemIndex == 0) {
    outReturnToMainMenu = true ;
  }else{
    tft.fillScreen (TFT_BLACK) ;
    switch (gDisplayPhase) {
    case DisplayPhase::list :
      { const String fileName = programFileNameAtIndex (gSelectedItemIndex - 1) ;
        const bool ok = readProgramFile (fileName) ;
        Serial.printf ("Read '%s' --> %d\n", fileName.c_str (), ok) ;
        if (ok) {
          gDisplayPhase = DisplayPhase::graph ;
          plotGraph () ;
        }
      } break ;
    case DisplayPhase::graph :
      printTable () ;
      gDisplayPhase = DisplayPhase::table ;
      break ;
    case DisplayPhase::table :
      gDisplayPhase = DisplayPhase::list ;
      setEncoderRange (0, gSelectedItemIndex, 0, true) ;
      break ;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
