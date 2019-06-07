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
//   ENTER PROGRAM MODE
//----------------------------------------------------------------------------------------------------------------------

void enterProgramMode (void) ;

//----------------------------------------------------------------------------------------------------------------------
//   PRINT PROGRAM MODE SCREEN
//----------------------------------------------------------------------------------------------------------------------

void printProgramModeScreen (void) ;

//----------------------------------------------------------------------------------------------------------------------
//   HANDLE ROTARY ENCODER IN PROGRAM MODE
//----------------------------------------------------------------------------------------------------------------------

void handleRotaryEncoderInProgramMode (void) ;

//----------------------------------------------------------------------------------------------------------------------
//   CLICK IN PROGRAM MODE
//----------------------------------------------------------------------------------------------------------------------

void clickInProgramMode (bool & outReturnToMainMenu) ;

//----------------------------------------------------------------------------------------------------------------------