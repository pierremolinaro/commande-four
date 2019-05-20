#include "ManualMode.h"
#include "TFT.h"
#include "TemperatureSensor.h"
#include "RotaryEncoder.h"
#include "MinFreeHeap.h"
#include "OvenControl.h"
#include "RealTimeClock.h"

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   STATIC VARIABLES
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static uint32_t gConsigneModeManuel ;
static bool gConsigneSelectionnee ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   ENTER MANUAL MODE
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void enterManualMode (void) {
  const int32_t v = lround (getSensorTemperature ()) - 10 ;
  if (v >= 0) {
    gConsigneModeManuel = (uint32_t) v ;
  }else{
    gConsigneModeManuel = 0 ;
  }
  gConsigneSelectionnee = false ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   PRINT MANUAL MODE SCREEN
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void printManualModeScreen (const uint8_t inIndiceSousMenuSelectionne) {
  setLign (0, 3) ;
  tft.setTextSize (3) ;
  setColumn (4, 3) ;
  tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
  tft.print ("Mode Manuel") ;

  setLign (3, 2) ;
  tft.setTextSize (2) ;
  tft.print (" Consigne : ") ;
  setMenuColor (inIndiceSousMenuSelectionne == 0) ;
  tft.setTextSize (gConsigneSelectionnee ? 3 : 2) ;
  tft.printf ("%4d" DEGREE_CHAR "C", gConsigneModeManuel) ;

  setLign (5, 2) ;
  tft.setTextSize (2) ;
  setMenuColor (inIndiceSousMenuSelectionne == 1) ;
  tft.print (ovenIsRunning () ? (" Arr" LOWERCASE_E_CIRCUM "ter four") : (" D" LOWERCASE_E_ACUTE "marrer four")) ;
  tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
  tft.print (" ") ;

  if (ovenIsRunning ()) {
    const uint32_t runningTime = ovenRunningTime () ;
    const uint32_t seconds = runningTime % 60 ;
    const uint32_t minutes = (runningTime / 60) % 60 ;
    const uint32_t hours = runningTime / 3600 ;
    tft.printf ("%02u:%02u:%02u", hours, minutes, seconds) ;
  }
  setLign (7, 2) ;
  setMenuColor (inIndiceSousMenuSelectionne == 2) ;
  tft.print (" Retour") ;

  setLign (10, 2) ;
  tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
  tft.print (" Indicateurs : ") ;
  printColoredStatus (getFaultlySampleCount ()) ;
  tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
  tft.print (" ") ;
  printColoredStatus (getRejectedInconsistentSampleCount ()) ;

  tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
  setLign (11, 2) ;
  tft.printf (" RAM : %u octets", minFreeHeap ()) ;
}

//----------------------------------------------------------------------------------------------------------------------
//   CLICK IN MANUAL MODE
//----------------------------------------------------------------------------------------------------------------------

void clickInManualMode (const uint8_t inIndiceSousMenuSelectionne,
                        bool & outRevenirPagePrincipale,
                        bool & outSaisirConsigne) {
  if (inIndiceSousMenuSelectionne == 0) {
    gConsigneSelectionnee = true ;
    outSaisirConsigne = true ;
    setLign (3, 2) ;
    setColumn (12, 2) ;
    tft.setTextSize (3) ;
    tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
    tft.print ("      ") ;
    setEncoderRange (0, gConsigneModeManuel, 1100) ;
  }else if (inIndiceSousMenuSelectionne == 1) {
    if (ovenIsRunning ()) {
      stopOven () ;
    }else{
      startOvenInManualMode (gConsigneModeManuel, currentDateTime ()) ;
    }
  }else if (inIndiceSousMenuSelectionne == 2) {
    stopOven () ;
    outRevenirPagePrincipale = true ;
  }
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void quitterModeReglageConsigneModeManuel (void) {
  gConsigneSelectionnee = false ;
  setLign (3, 2) ;
  setColumn (12, 2) ;
  tft.setTextSize (3) ;
  tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
  tft.print ("      ") ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   SET MANUAL MODE TEMPERATURE REFERENCE
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void setTemperatureInManualMode (void) {
  gConsigneModeManuel = getEncoderValue () ;
  setTemperatureReferenceInManualMode (gConsigneModeManuel) ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
