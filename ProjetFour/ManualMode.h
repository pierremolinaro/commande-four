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
//   ENTER MANUAL MODE
//----------------------------------------------------------------------------------------------------------------------

void enterManualMode (void) ;

//----------------------------------------------------------------------------------------------------------------------
//   PRINT MANUAL MODE SCREEN
//----------------------------------------------------------------------------------------------------------------------

void printManualModeScreen (const uint8_t inIndiceSousMenuSelectionne) ;

//----------------------------------------------------------------------------------------------------------------------
//   CLICK IN MANUAL MODE
//----------------------------------------------------------------------------------------------------------------------

void clickInManualMode (const uint8_t inIndiceSousMenuSelectionne,
                        bool & outRevenirPagePrincipale,
                        bool & outSaisirConsigne) ;

//----------------------------------------------------------------------------------------------------------------------
//   REGLAGE CONSIGNE MODE MANUEL
//----------------------------------------------------------------------------------------------------------------------

void reglageConsigneModeManuel (void) ;

void quitterModeReglageConsigneModeManuel (void) ;

//----------------------------------------------------------------------------------------------------------------------
