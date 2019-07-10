#include "ManualMode.h"
#include "TFT.h"
#include "TemperatureSensor.h"
#include "RotaryEncoder.h"
#include "MinFreeHeap.h"
#include "OvenControl.h"
#include "RealTimeClock.h"
#include "gcc-diagnostics.h"

//----------------------------------------------------------------------------------------------------------------------
//   STATIC VARIABLES
//----------------------------------------------------------------------------------------------------------------------

static uint32_t gSelectedItemIndex ;
static uint32_t gTemperatureReference ;
static bool gTemperatureReferenceSettingSelected ;

//----------------------------------------------------------------------------------------------------------------------
//   ENTER MANUAL MODE
//----------------------------------------------------------------------------------------------------------------------

void enterManualMode (void) {
  const int32_t v = lround (getSensorTemperature ()) - 10 ;
  if (v >= 0) {
    gTemperatureReference = (uint32_t) v ;
  }else{
    gTemperatureReference = 0 ;
  }
  gTemperatureReferenceSettingSelected = false ;
  gSelectedItemIndex = 0 ;
  setEncoderRange (0, gSelectedItemIndex, 2, true) ;
}

//----------------------------------------------------------------------------------------------------------------------
//   PRINT MANUAL MODE SCREEN
//----------------------------------------------------------------------------------------------------------------------

void printManualModeScreen (void) {
  tft.setTextSize (3) ;
  setLineColumnForTextSize (0, 1, 3) ;
  setMenuColor (gSelectedItemIndex == 2, false) ;
  tft.print ("Retour") ;

  setLineColumnForTextSize (2, 1, 3) ;
  tft.setTextColor (ILI9341_WHITE, ILI9341_BLACK) ;
  tft.print ("Consigne ") ;
  setMenuColor (gSelectedItemIndex == 0, gTemperatureReferenceSettingSelected) ;
  tft.printf ("%4d" DEGREE_CHAR "C", gTemperatureReference) ;

  setLineColumnForTextSize (4, 1, 3) ;
  tft.setTextColor (ILI9341_WHITE, ILI9341_BLACK) ;
  tft.print ("Four ") ;
  setMenuColor (gSelectedItemIndex == 1, false) ;
  tft.print (ovenIsRunning () ? ("Arr" LOWERCASE_E_CIRCUM "ter") : ("D" LOWERCASE_E_ACUTE "marrer")) ;
  tft.setTextColor (ILI9341_WHITE, ILI9341_BLACK) ;
  tft.print (" ") ;

  if (ovenIsRunning ()) {
    tft.setTextSize (3) ;
    setLineColumnForTextSize (0, 8, 3) ;
    const uint32_t runningTime = ovenRunningTime () ;
    const uint32_t seconds = runningTime % 60 ;
    const uint32_t minutes = (runningTime / 60) % 60 ;
    const uint32_t hours = runningTime / 3600 ;
    tft.printf ("%02u:%02u:%02u", hours, minutes, seconds) ;
  }

  setLineColumnForTextSize (10, 1, 2) ;
  tft.setTextSize (2) ;
  tft.setTextColor (ILI9341_WHITE, ILI9341_BLACK) ;
  tft.print ("Indicateurs : ") ;
  printColoredStatus (getFaultlySampleCount ()) ;
  tft.setTextColor (ILI9341_WHITE, ILI9341_BLACK) ;
  tft.print (" ") ;
  printColoredStatus (getRejectedInconsistentSampleCount ()) ;

  tft.setTextColor (ILI9341_WHITE, ILI9341_BLACK) ;
  setLineColumnForTextSize (11, 1, 2) ;
  tft.printf ("RAM : %u octets", minFreeHeap ()) ;
}

//----------------------------------------------------------------------------------------------------------------------
//   ROTARY ENCODER IN MANUAL MODE
//----------------------------------------------------------------------------------------------------------------------

void handleRotaryEncoderInManualMode (void) {
  if (gTemperatureReferenceSettingSelected) {
    gTemperatureReference = getEncoderValue () ;
    setTemperatureReferenceInManualMode (gTemperatureReference) ;
  }else{
    gSelectedItemIndex = getEncoderValue () ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
//   CLICK IN MANUAL MODE
//----------------------------------------------------------------------------------------------------------------------

void clickInManualMode (bool & outReturnToMainMenu) {
  if (gSelectedItemIndex == 0) {
    gTemperatureReferenceSettingSelected ^= true ;
    if (gTemperatureReferenceSettingSelected) {
      setEncoderRange (0, gTemperatureReference, 1100, false) ;
    }else{
      setEncoderRange (0, gSelectedItemIndex, 2, true) ;
    }
  }else if (gSelectedItemIndex == 1) {
    if (ovenIsRunning ()) {
      stopOven () ;
    }else{
      startOvenInManualMode (gTemperatureReference) ;
    }
  }else if (gSelectedItemIndex == 2) {
    stopOven () ;
    outReturnToMainMenu = true ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
