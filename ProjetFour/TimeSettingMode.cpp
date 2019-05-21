//----------------------------------------------------------------------------------------------------------------------
//  INCLUDES
//----------------------------------------------------------------------------------------------------------------------

#include "TimeSettingMode.h"
#include "RealTimeClock.h"
#include "TFT.h"
#include "RotaryEncoder.h"

//----------------------------------------------------------------------------------------------------------------------
//   STATIC VARIABLES
//----------------------------------------------------------------------------------------------------------------------

static uint32_t gSelectedItemIndex ;
static uint32_t gEditingItemIndex ;

//----------------------------------------------------------------------------------------------------------------------
//   ENTER TIME SETTING MODE
//----------------------------------------------------------------------------------------------------------------------

void enterTimeSettingMode (void) {
  gSelectedItemIndex = 0 ;
  gEditingItemIndex = 0 ;
  setEncoderRange (0, 0, 5, true) ;
}

//----------------------------------------------------------------------------------------------------------------------
//   PRINT TIME SETTING MODE SCREEN
//----------------------------------------------------------------------------------------------------------------------

void printTimeSettingModeScreen (void) {
  const RtcDateTime now = currentDateTime () ;
// ----------Return----------
  setLign(0, 2); tft.setTextSize(2) ;
  setMenuColor (gSelectedItemIndex == 0, false) ;  
  tft.print ("Retour") ;
// ----------Date----------
  setLign (2, 3); setColumn (1, 3) ; tft.setTextSize(3) ;
  setMenuColor (gSelectedItemIndex == 1, gEditingItemIndex == 1) ;  
  tft.printf ("%02u", now.Day ()) ;
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.print (":") ;
  setMenuColor (gSelectedItemIndex == 2, gEditingItemIndex == 2) ;  
  tft.printf ("%02u", now.Month ()) ;
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.print (":") ;
  setMenuColor (gSelectedItemIndex == 3, gEditingItemIndex == 3) ;  
  tft.printf ("%04u", now.Year ()) ;
// ----------Time----------
  setLign(4, 3) ; setColumn (1, 3) ;
  setMenuColor (gSelectedItemIndex == 4, gEditingItemIndex == 4) ;  
  tft.printf ("%02u", now.Hour ()) ;
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.print (":") ;
  setMenuColor (gSelectedItemIndex == 5, gEditingItemIndex == 5) ;  
  tft.printf ("%02u", now.Minute ()) ;
}

//----------------------------------------------------------------------------------------------------------------------

static bool isLeapYear (uint16_t inYear) {
  return ((inYear%4 == 0) && (inYear%100 != 0)) || (inYear%400 == 0) ;
}

static const uint8_t monthDays [12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 } ; // For a non leap year

//----------------------------------------------------------------------------------------------------------------------
//   HANDLE ROTARY ENCODER IN TIME SETTING MODE
//----------------------------------------------------------------------------------------------------------------------

void handleRotaryEncoderInTimeSettingMode (void) {
  switch (gEditingItemIndex) {
  case 0 :
    gSelectedItemIndex = getEncoderValue () ;
    break ;
  case 1 : // Day
    setDay (getEncoderValue ()) ;
    break ;
  case 2 : // Month
    setMonth (getEncoderValue ()) ;
    break ;
  case 3 : // Year
    setYear (getEncoderValue ()) ;
    break ;
  case 4 : // Hour
    setHour (getEncoderValue ()) ;
    break ;
  case 5 : // Minute
    setMinute (getEncoderValue ()) ;
    break ;
  default : break ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
//   CLICK IN TIME SETTING MODE
//----------------------------------------------------------------------------------------------------------------------

void clickInTimeSettingMode (bool & outReturnToMainMenu) {
  if (gSelectedItemIndex == 0) {
    outReturnToMainMenu = true ;
  }else if (gEditingItemIndex == 0) {
    gEditingItemIndex = gSelectedItemIndex ;
    const RtcDateTime now = currentDateTime () ;
    switch (gEditingItemIndex) {
    case 1 : // Day
      { uint32_t daysInMonth = monthDays [now.Month () - 1] ;
        if (isLeapYear (now.Year ()) && (now.Month () == 2)) {
          daysInMonth += 1 ;
        }
        setEncoderRange (1, now.Day (), daysInMonth, true) ;
     }break ;
    case 2 : // Month
      setEncoderRange (1, now.Month (), 12, true) ;
      break ;
    case 3 : // Year
      setEncoderRange (2000, now.Year (), 2099, false) ;
      break ;
    case 4 : // Hour
      setEncoderRange (0, now.Hour (), 23, true) ;
      break ;
    case 5 : // Minute
      setEncoderRange (0, now.Minute (), 59, true) ;
      break ;
    default : break ;
    }
  }else{
    setEncoderRange (0, gSelectedItemIndex, 5, true) ;
    gEditingItemIndex = 0 ; 
  }
}

//----------------------------------------------------------------------------------------------------------------------
