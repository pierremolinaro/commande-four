//----------------------------------------------------------------------------------------------------------------------
//  INCLUDES
//----------------------------------------------------------------------------------------------------------------------

#include "ProgramMode.h"
#include "ProgramListMode.h"
#include "RemoveFileMode.h"
#include "RotaryEncoder.h"
#include "TFT.h"

//----------------------------------------------------------------------------------------------------------------------
//  SUB MODES ENUMERATION
//----------------------------------------------------------------------------------------------------------------------

enum class ProgramSubMode { main, listPrgms, newPrgm, supprPrgm } ;

//----------------------------------------------------------------------------------------------------------------------
//  STATIC VARIABLES
//----------------------------------------------------------------------------------------------------------------------

static uint32_t gSelectedItemIndex ;
static ProgramSubMode gSubMode = ProgramSubMode::main ;

//----------------------------------------------------------------------------------------------------------------------
//   ENTER PROGRAM MODE
//----------------------------------------------------------------------------------------------------------------------

void enterProgramMode (void) {
  gSubMode = ProgramSubMode::main ;
  gSelectedItemIndex = 0 ;
  setEncoderRange (0, gSelectedItemIndex, 3, true) ;
}

//----------------------------------------------------------------------------------------------------------------------
//   PRINT PROGRAM MODE SCREEN
//----------------------------------------------------------------------------------------------------------------------

void printProgramModeScreen (void) {
  switch (gSubMode) {
  case ProgramSubMode::main :
    tft.setTextSize (3) ;
    setLineColumnForTextSize (0, 1, 3) ;
    setMenuColor (gSelectedItemIndex == 0, false) ;
    tft.print ("Retour") ;
    setLineColumnForTextSize (2, 1, 3) ;
    setMenuColor (gSelectedItemIndex == 1, false) ;
    tft.print ("Infos programmes") ;
    setLineColumnForTextSize (4, 1, 3) ;
    setMenuColor (gSelectedItemIndex == 2, false) ;
    tft.print ("Nouv. programme") ;
    setLineColumnForTextSize (6, 1, 3) ;
    setMenuColor (gSelectedItemIndex == 3, false) ;
    tft.print ("Suppr. programme") ;
    break ;
  case ProgramSubMode::listPrgms :
    printProgramListModeScreen () ;
    break ;
  case ProgramSubMode::newPrgm :
    break ;
  case ProgramSubMode::supprPrgm :
    printRemoveProgramModeScreen () ;
    break ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
//   HANDLE ROTARY ENCODER IN PROGRAM MODE
//----------------------------------------------------------------------------------------------------------------------

void handleRotaryEncoderInProgramMode (void) {
  switch (gSubMode) {
  case ProgramSubMode::main :
    gSelectedItemIndex = getEncoderValue () ;
    break ;
  case ProgramSubMode::listPrgms :
    handleRotaryEncoderInProgramListMode () ;
    break ;
  case ProgramSubMode::newPrgm :
    break ;
  case ProgramSubMode::supprPrgm :
    handleRotaryEncoderInRemoveProgramMode () ;
    break ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
//   CLICK IN PROGRAM MODE
//----------------------------------------------------------------------------------------------------------------------

void clickInProgramMode (bool & outReturnToMainMenu) {
  bool returnToProgramMainMenu = false ;
  switch (gSubMode) {
  case ProgramSubMode::main :
    switch (gSelectedItemIndex) {
    case 0 :
      outReturnToMainMenu = true ;
      break ;
    case 1 :
      enterProgramListMode () ;
      gSubMode = ProgramSubMode::listPrgms ;
      break ;
    case 3 :
      enterRemoveProgramMode () ;
      gSubMode = ProgramSubMode::supprPrgm ;
      break ;
    default :
      break ;
    }
    break ;
  case ProgramSubMode::listPrgms :
    clickInProgramListMode (returnToProgramMainMenu) ;
    break ;
  case ProgramSubMode::newPrgm :
    break ;
  case ProgramSubMode::supprPrgm :
    clickInRemoveProgramMode (returnToProgramMainMenu) ;
    break ;
  }
  if (returnToProgramMainMenu) {
    gSubMode = ProgramSubMode::main ;
    setEncoderRange (0, gSelectedItemIndex, 3, true) ;
    tft.fillScreen (TFT_BLACK) ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
