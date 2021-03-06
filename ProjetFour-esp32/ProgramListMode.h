#pragma once

//----------------------------------------------------------------------------------------------------------------------
//  Check board type
//----------------------------------------------------------------------------------------------------------------------

#ifndef ARDUINO_MH_ET_LIVE_ESP32MINIKIT
  #error "Select 'MH ET LIVE ESP32MiniKit' board"
#endif

//----------------------------------------------------------------------------------------------------------------------

#include <Arduino.h>

//----------------------------------------------------------------------------------------------------------------------
//   ENTER PROGRAM LIST MODE
//----------------------------------------------------------------------------------------------------------------------

void enterProgramListMode (void) ;

//----------------------------------------------------------------------------------------------------------------------
//   PRINT PROGRAM LIST MODE SCREEN
//----------------------------------------------------------------------------------------------------------------------

void printProgramListModeScreen (void) ;

//----------------------------------------------------------------------------------------------------------------------
//   HANDLE ROTARY ENCODER IN PROGRAM LISTMODE
//----------------------------------------------------------------------------------------------------------------------

void handleRotaryEncoderInProgramListMode (void) ;

//----------------------------------------------------------------------------------------------------------------------
//   CLICK IN PROGRAM LIST MODE
//----------------------------------------------------------------------------------------------------------------------

void clickInProgramListMode (bool & outReturnToMainMenu) ;

//----------------------------------------------------------------------------------------------------------------------
