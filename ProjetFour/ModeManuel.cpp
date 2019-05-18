#include "ModeManuel.h"
#include "TFT.h"
#include "Temp_Sensor.h"
#include "encoder.h"
#include "FreeHeap.h"

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   VARIABLES INTERNES AU MODE MANUEL
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static uint32_t gConsigneModeManuel ;
static bool gFourEnMarche ;
static bool gConsigneSelectionnee ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   ENTREE DANS LE MODE MANUEL
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void entreeModeManuel (void) {
  const int32_t v = lround (getTemp ()) - 10 ;
  if (v >= 0) {
    gConsigneModeManuel = (uint32_t) v ;
  }else{
    gConsigneModeManuel = 0 ;
  }
  gFourEnMarche = false ;
  gConsigneSelectionnee = false ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   IMPRIMER ECRAN MODE MANUEL
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void imprimerEcranModeManuel (const uint8_t inIndiceSousMenuSelectionne) {
  setLign (0, 3) ;
  tft.setTextSize (3) ;
  setColumn (3, 3) ;
  tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
  tft.print (" Mode Manuel") ;

  setLign (3, 2) ;
  tft.setTextSize (2) ;
  tft.print (" Consigne : ") ;
  setMenuColor (inIndiceSousMenuSelectionne == 0) ;
  tft.setTextSize (gConsigneSelectionnee ? 3 : 2) ;
  tft.printf ("%4d" DEGREE "C", gConsigneModeManuel) ;

  setLign (5, 2) ;
  tft.setTextSize (2) ;
  setMenuColor (inIndiceSousMenuSelectionne == 1) ;
  tft.print (gFourEnMarche ? (" Arr" E_MIN_CIRC "ter four") : (" D" E_MIN_AIGU "marrer four")) ;
  tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
  tft.print (" ") ;

  setLign (7, 2) ;
  setMenuColor (inIndiceSousMenuSelectionne == 2) ;
  tft.print (" Retour") ;

  setLign (10, 2) ;
  tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
  tft.print (" Indicateurs : ") ;
  printColoredStatus (obtenirNombreMesuresBrutesIncorrectes ()) ;
  tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
  tft.print (", ") ;
  printColoredStatus (obtenirNombreMesuresBrutesIncoherentesRejetees ()) ;
 
  tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
  setLign (11, 2) ;
  tft.printf (" RAM : %u octets", ramLibre ()) ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   EXECUTER ECRAN MODE MANUEL
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void actionModeManuel (const uint8_t inIndiceSousMenuSelectionne,
                       bool & outRevenirPagePrincipale,
                       bool & outSaisirConsigne) {
  if (inIndiceSousMenuSelectionne == 0) {
    gConsigneSelectionnee = true ;
    outSaisirConsigne = gConsigneSelectionnee ;
    setLign (3, 2) ;
    setColumn (12, 2) ;
    tft.setTextSize (3) ;
    tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
    tft.print ("      ") ;
    if (gConsigneSelectionnee) {
      definirPlageEncodeur (0, gConsigneModeManuel, 1100) ;
    }
  }else if (inIndiceSousMenuSelectionne == 1) {
    gFourEnMarche ^= true ;
  }else if (inIndiceSousMenuSelectionne == 2) {
    outRevenirPagePrincipale = true ;
    gFourEnMarche = false ;
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
//   REGLAGE CONSIGNE MODE MANUEL
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void reglageConsigneModeManuel (void) {
  gConsigneModeManuel = getEncoderValue () ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
