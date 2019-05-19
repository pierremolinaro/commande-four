#pragma once

#include <Arduino.h>

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   ENTREE DANS LE MODE MANUEL
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void entreeModeManuel (void) ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   IMPRIMER ECRAN MODE MANUEL
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void imprimerEcranModeManuel (const uint8_t inIndiceSousMenuSelectionne) ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   ACTION MODE MANUEL
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void actionModeManuel (const uint8_t inIndiceSousMenuSelectionne,
                       bool & outRevenirPagePrincipale,
                       bool & outSaisirConsigne) ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   REGLAGE CONSIGNE MODE MANUEL
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void reglageConsigneModeManuel (void) ;

void quitterModeReglageConsigneModeManuel (void) ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————