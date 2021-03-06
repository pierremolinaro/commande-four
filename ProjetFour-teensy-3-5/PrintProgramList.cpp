//----------------------------------------------------------------------------------------------------------------------
//  INCLUDES
//----------------------------------------------------------------------------------------------------------------------

#include "PrintProgramList.h"
#include "ProgramFiles.h"
#include "TFT.h"
#include "SDCard.h"
#include "RotaryEncoder.h"
#include "gcc-diagnostics.h"

//----------------------------------------------------------------------------------------------------------------------
//   CONSTANTS
//----------------------------------------------------------------------------------------------------------------------

static const uint32_t NAMES_IN_PAGES = 8 ;
static const char * const INSERT_SD_CARD_MESSAGE = "Ins" LOWERCASE_E_ACUTE "rer carte SD" ;
static uint32_t gDisplayOffset ;

//----------------------------------------------------------------------------------------------------------------------
//   STATIC VARIABLES
//----------------------------------------------------------------------------------------------------------------------

static bool gSDCardIsMounted ;

//----------------------------------------------------------------------------------------------------------------------
//   STRING UTILITY
//----------------------------------------------------------------------------------------------------------------------

static String stringWithSpaces (const uint32_t inCount) {
  String result ;
  for (uint32_t i=0 ; i<inCount ; i++) {
    result.concat (' ') ;
  }
  return result ;
}

//----------------------------------------------------------------------------------------------------------------------
//   PRINT PROGRAM LIST
//----------------------------------------------------------------------------------------------------------------------

void printProgramList (uint32_t & ioSelectedItemIndex, const char * inTitle) {
  tft.setTextSize (3) ;
  setLineColumnForTextSize (0, 1, 3) ;
  setMenuColor (ioSelectedItemIndex == 0, false) ;
  tft.print ("Retour") ;
  tft.setTextColor (ILI9341_WHITE, ILI9341_BLACK) ;
  tft.print (" ") ; tft.print (inTitle) ;
//--- Testing SD Card
  const bool sdCardIsMounted = sdCardStatus () == SDCardStatus::mounted ;
//--- Erase file names, or no card display ?
  if (gSDCardIsMounted && !sdCardIsMounted) {
    tft.setTextSize (2) ;
    tft.setTextColor (ILI9341_WHITE, ILI9341_BLACK) ;
    const String pad = stringWithSpaces (FILE_BASE_NAME_MAX_LENGTH) ;
    for (uint32_t i = 0 ; i < (NAMES_IN_PAGES + 2) ; i++) {
      setLineColumnForTextSize (2 + i, 1, 2) ;
      tft.print (pad) ;
    }
  }else if (sdCardIsMounted && !gSDCardIsMounted) {
    setLineColumnForTextSize (2, 1, 3) ;
    tft.setTextSize (3) ;
    tft.setTextColor (ILI9341_WHITE, ILI9341_BLACK) ;
    tft.print (stringWithSpaces (strlen (INSERT_SD_CARD_MESSAGE))) ;
  }
//---
  gSDCardIsMounted = sdCardIsMounted ;
  if (gSDCardIsMounted) {
    buildProgramFileNameArray () ;
    setLineColumnForTextSize (2, 2, 2) ;
    tft.setTextSize (2) ;
    tft.setTextColor (ILI9341_WHITE, ILI9341_BLACK) ;
  //--- Update offset
    if (programFileCount () <= NAMES_IN_PAGES) {
      gDisplayOffset = 0 ;
    }else{
      if (ioSelectedItemIndex < NAMES_IN_PAGES / 2) {
        gDisplayOffset = 0 ;
      }else if (ioSelectedItemIndex > (programFileCount () - NAMES_IN_PAGES / 2)) {
        gDisplayOffset = programFileCount () - NAMES_IN_PAGES ;
      }else{
        gDisplayOffset = ioSelectedItemIndex - 1 - NAMES_IN_PAGES / 2 ;
      }
    }
    setLineColumnForTextSize (2, 1, 2) ;
    tft.setTextColor (ILI9341_WHITE, ILI9341_BLACK) ;
    tft.print ((gDisplayOffset == 0) ? "   " : "...") ;
    const uint32_t displayCount = min (programFileCount (), NAMES_IN_PAGES) ;
    for (uint32_t i = 0 ; i < displayCount ; i++) {
      setLineColumnForTextSize (3 + i, 1, 2) ;
      setMenuColor (ioSelectedItemIndex == (i + gDisplayOffset + 1), false) ;
      const String fileName = programFileNameAtIndex (i + gDisplayOffset) ;
      tft.print (fileName) ;
      tft.setTextColor (ILI9341_WHITE, ILI9341_BLACK) ;
      uint32_t x = fileName.length () ;
      while (x < FILE_BASE_NAME_MAX_LENGTH) {
        x += 1 ;
        tft.print (" ") ;
      }
    }
    const bool displayFooter = (gDisplayOffset + displayCount) < programFileCount () ;
    setLineColumnForTextSize (3 + NAMES_IN_PAGES, 1, 2) ;
    tft.setTextColor (ILI9341_WHITE, ILI9341_BLACK) ;
    tft.print (displayFooter ? "..." : "   ") ;
    if (getEncoderMaxValue () != programFileCount ()) {
      setEncoderRange (0, ioSelectedItemIndex, programFileCount (), true) ;
    }
  }else{
    setLineColumnForTextSize (2, 1, 3) ;
    tft.setTextSize (3) ;
    tft.setTextColor (ILI9341_RED, ILI9341_BLACK) ;
    tft.print (INSERT_SD_CARD_MESSAGE) ;
    ioSelectedItemIndex = 0 ;
    setEncoderRange (0, 0, 0, true) ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
