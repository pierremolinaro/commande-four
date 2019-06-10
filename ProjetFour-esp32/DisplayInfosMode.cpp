#include "DisplayInfosMode.h"
#include "TFT.h"
#include "TemperatureSensor.h"
#include "MinFreeHeap.h"
#include "gcc-diagnostics.h"

//----------------------------------------------------------------------------------------------------------------------

void displayInfos (void) {
// ----------Return----------
  setLineColumnForTextSize (0, 0, 2) ;
  tft.setTextSize (2) ;
  setMenuColor (true, false) ;
  tft.print ("Retour") ;
// ----------Temperature----------
  setLineColumnForTextSize (3, 1, 2) ;
  tft.setTextColor (TFT_WHITE, TFT_BLACK);
  tft.printf ("Temp" LOWERCASE_E_ACUTE "rature : %7.2f" DEGREE_CHAR "C", getSensorTemperature ()) ;
//--- Nombre de mesures incorrectes
  setLineColumnForTextSize (4, 1, 2) ;
  afficherCompteurErreurs ("Mesures invalides : ", getFaultlySampleCount ()) ;
//--- Nombre de mesures rejetées
  setLineColumnForTextSize (5, 1, 2) ;
  afficherCompteurErreurs ("Mesures rejet" "\x82" "es : ", getRejectedInconsistentSampleCount ()) ;
//--- Nombre de moyennes invalides
  setLineColumnForTextSize (6, 1, 2) ;
  afficherCompteurErreurs ("Moyennes invalides : ", obtenirNombreMesuresMoyennesInvalides ()) ;
//----- Afficher la RAM libre
  setLineColumnForTextSize (7, 1, 2) ;
  tft.setTextColor (TFT_WHITE, TFT_BLACK);
  tft.printf ("RAM libre %6u octets", minFreeHeap ()) ;
}

//----------------------------------------------------------------------------------------------------------------------