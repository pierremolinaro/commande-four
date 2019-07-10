//----------------------------------------------------------------------------------------------------------------------
//  INCLUDES
//----------------------------------------------------------------------------------------------------------------------

#include "PrintProgramList.h"
#include "RemoveFileMode.h"
#include "ProgramFiles.h"
#include "TFT.h"
#include "SDCard.h"
#include "RotaryEncoder.h"
#include "gcc-diagnostics.h"

//----------------------------------------------------------------------------------------------------------------------
//   STATIC VARIABLES
//----------------------------------------------------------------------------------------------------------------------

static uint32_t gSelectedItemIndex ;
static uint32_t gSelectedDialogItemIndex ;
static bool gSDCardIsMounted ;

enum class DisplayPhase { list, dialog } ;

static DisplayPhase gDisplayPhase = DisplayPhase::list ;

//----------------------------------------------------------------------------------------------------------------------
//   ENTER PROGRAM LIST MODE
//----------------------------------------------------------------------------------------------------------------------

void enterRemoveProgramMode (void) {
  tft.fillScreen (ILI9341_BLACK) ;
  gSDCardIsMounted = false ;
  gDisplayPhase = DisplayPhase::list ;
  gSelectedItemIndex = 0 ;
  setEncoderRange (0, gSelectedItemIndex, 0, true) ;
}

//----------------------------------------------------------------------------------------------------------------------

static void displayRemoveDialog (void) {
  tft.setTextSize (2) ;
  setLineColumnForTextSize (1, 1, 3) ;
  tft.setTextColor (ILI9341_WHITE, ILI9341_BLACK) ;
  tft.print ("Fichier") ;
  setLineColumnForTextSize (2, 1, 3) ;
  tft.print (programFileNameAtIndex (gSelectedItemIndex - 1)) ;
//---
  setLineColumnForTextSize (4, 1, 3) ;
  setMenuColor (gSelectedDialogItemIndex == 0, false) ;
  tft.print ("Ne pas supprimer") ;
  setLineColumnForTextSize (6, 1, 3) ;
  setMenuColor (gSelectedDialogItemIndex == 1, false) ;
  tft.print ("Supprimer") ;
}

//----------------------------------------------------------------------------------------------------------------------

void printRemoveProgramModeScreen (void) {
  switch (gDisplayPhase) {
  case DisplayPhase::list :
    printProgramList (gSelectedItemIndex, "Suppr.") ;
    break ;
  case DisplayPhase::dialog :
    displayRemoveDialog () ;
    break ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
//   HANDLE ROTARY ENCODER IN REMOVE PROGRAM MODE
//----------------------------------------------------------------------------------------------------------------------

void handleRotaryEncoderInRemoveProgramMode (void) {
  switch (gDisplayPhase) {
  case DisplayPhase::list :
    gSelectedItemIndex = getEncoderValue () ;
    break ;
  case DisplayPhase::dialog :
    gSelectedDialogItemIndex = getEncoderValue () ;
    break ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
//   CLICK IN REMOVE PROGRAM MODE
//----------------------------------------------------------------------------------------------------------------------

void clickInRemoveProgramMode (bool & outReturnToMainMenu) {
  if (gSelectedItemIndex == 0) {
    outReturnToMainMenu = true ;
  }else{
    tft.fillScreen (ILI9341_BLACK) ;
    switch (gDisplayPhase) {
    case DisplayPhase::list :
      gSelectedDialogItemIndex = 0 ;
      setEncoderRange (0, gSelectedDialogItemIndex, 1, true) ;
      gDisplayPhase = DisplayPhase::dialog ;
      break ;
    case DisplayPhase::dialog :
      if (gSelectedDialogItemIndex == 1) {
        const String fileName = programFileNameAtIndex (gSelectedItemIndex - 1) ;
        const String filePath = String (PROFILES_DIRECTORY) + '/' + fileName + ".CSV" ;
        const bool ok = SD.remove (filePath.c_str ()) ;
        Serial.printf ("Remove file '%s': %s\n", filePath.c_str (), ok ? "done" : "error") ;
        if (ok) {
          resetProgramList () ;
        }
      }
      gDisplayPhase = DisplayPhase::list ;
      setEncoderRange (0, gSelectedItemIndex, 0, true) ;
      break ;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
