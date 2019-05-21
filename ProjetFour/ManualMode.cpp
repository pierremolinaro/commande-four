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

static uint32_t gMenuItemIndex ;
static uint32_t gTemperatureReference ;
static bool gTemperatureReferenceSettingSelected ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   ENTER MANUAL MODE
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void enterManualMode (void) {
  const int32_t v = lround (getSensorTemperature ()) - 10 ;
  if (v >= 0) {
    gTemperatureReference = (uint32_t) v ;
  }else{
    gTemperatureReference = 0 ;
  }
  gTemperatureReferenceSettingSelected = false ;
  gMenuItemIndex = 0 ;
  setEncoderRange (0, 0, 2, true) ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   PRINT MANUAL MODE SCREEN
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void printManualModeScreen (void) {
  setLign (0, 3) ;
  tft.setTextSize (3) ;
  setColumn (4, 3) ;
  tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
  tft.print ("Mode manuel") ;

  setLign (3, 2) ;
  tft.setTextSize (2) ;
  tft.print (" Consigne : ") ;
  setMenuColor (gMenuItemIndex == 0) ;
  tft.setTextSize (gTemperatureReferenceSettingSelected ? 3 : 2) ;
  tft.printf ("%4d" DEGREE_CHAR "C", gTemperatureReference) ;

  setLign (5, 2) ;
  tft.setTextSize (2) ;
  setMenuColor (gMenuItemIndex == 1) ;
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
  setMenuColor (gMenuItemIndex == 2) ;
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
//   ROTARY ENCODER IN MANUAL MODE
//----------------------------------------------------------------------------------------------------------------------

void handleRotaryEncoderInManualMode (void) {
  if (gTemperatureReferenceSettingSelected) {
    gTemperatureReference = getEncoderValue () ;
    setTemperatureReferenceInManualMode (gTemperatureReference) ;
  }else{
    gMenuItemIndex = getEncoderValue () ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
//   CLICK IN MANUAL MODE
//----------------------------------------------------------------------------------------------------------------------

void clickInManualMode (bool & outReturnToMainMenu) {
  if (gMenuItemIndex == 0) {
    gTemperatureReferenceSettingSelected ^= true ;
    setLign (3, 2) ;
    setColumn (12, 2) ;
    tft.setTextSize (3) ;
    tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
    tft.print ("      ") ;
    if (gTemperatureReferenceSettingSelected) {
      setEncoderRange (0, gTemperatureReference, 1100, false) ;
    }else{
      setEncoderRange (0, gMenuItemIndex, 2, true) ;
    }
  }else if (gMenuItemIndex == 1) {
    if (ovenIsRunning ()) {
      stopOven () ;
    }else{
      startOvenInManualMode (gTemperatureReference, currentDateTime ()) ;
    }
  }else if (gMenuItemIndex == 2) {
    stopOven () ;
    outReturnToMainMenu = true ;
  }
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

//void quitterModeReglageConsigneModeManuel (void) {
//  gTemperatureReferenceSettingSelected = false ;
//  setLign (3, 2) ;
//  setColumn (12, 2) ;
//  tft.setTextSize (3) ;
//  tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
//  tft.print ("      ") ;
//}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   SET MANUAL MODE TEMPERATURE REFERENCE
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

//void setTemperatureInManualMode (void) {
//  gTemperatureReference = getEncoderValue () ;
//  setTemperatureReferenceInManualMode (gTemperatureReference) ;
//}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
