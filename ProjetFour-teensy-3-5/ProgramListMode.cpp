//----------------------------------------------------------------------------------------------------------------------
//  INCLUDES
//----------------------------------------------------------------------------------------------------------------------

#include "PrintProgramList.h"
#include "ProgramListMode.h"
#include "ProgramFiles.h"
#include "TFT.h"
#include "SDCard.h"
#include "RotaryEncoder.h"
#include "gcc-diagnostics.h"

//----------------------------------------------------------------------------------------------------------------------
//   STATIC VARIABLES
//----------------------------------------------------------------------------------------------------------------------

static uint32_t gSelectedItemIndex ;
static bool gSDCardIsMounted ;

enum class DisplayPhase { list, graph, table } ;

static DisplayPhase gDisplayPhase = DisplayPhase::list ;

//----------------------------------------------------------------------------------------------------------------------
//   ENTER PROGRAM LIST MODE
//----------------------------------------------------------------------------------------------------------------------

void enterProgramListMode (void) {
  tft.fillScreen (ILI9341_BLACK) ;
  gSDCardIsMounted = false ;
  gDisplayPhase = DisplayPhase::list ;
  gSelectedItemIndex = 0 ;
  setEncoderRange (0, gSelectedItemIndex, 0, true) ;
}

//----------------------------------------------------------------------------------------------------------------------

void printProgramListModeScreen (void) {
  switch (gDisplayPhase) {
  case DisplayPhase::list :
    printProgramList (gSelectedItemIndex, "Liste") ;
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
    tft.fillScreen (ILI9341_BLACK) ;
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
      printTable ("Retour") ;
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
