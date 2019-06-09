#include "DisplayInfosMode.h"
#include "TFT.h"
#include "TemperatureSensor.h"
#include "MinFreeHeap.h"

//----------------------------------------------------------------------------------------------------------------------

void displayInfos (void) {
// ----------Return----------
  setLineForTextSize (0, 2) ;
  tft.setTextSize (2) ;
  setMenuColor (true, false) ;
  tft.print ("Retour") ;
// ----------Temperature----------
  setLineForTextSize (3, 2) ; setColumnForTextSize (1, 2) ;
  tft.setTextColor (TFT_WHITE, TFT_BLACK);
  tft.printf ("Temp" LOWERCASE_E_ACUTE "rature : %7.2f" DEGREE_CHAR "C", getSensorTemperature ()) ;
//--- Nombre de mesures incorrectes
  setLineForTextSize (4, 2) ; setColumnForTextSize (1, 2) ;
  afficherCompteurErreurs ("Mesures invalides : ", getFaultlySampleCount ()) ;
//--- Nombre de mesures rejetées
  setLineForTextSize (5, 2) ; setColumnForTextSize (1, 23) ;
  afficherCompteurErreurs ("Mesures rejet" "\x82" "es : ", getRejectedInconsistentSampleCount ()) ;
//--- Nombre de moyennes invalides
  setLineForTextSize (6, 2) ; setColumnForTextSize (1, 2) ;
  afficherCompteurErreurs ("Moyennes invalides : ", obtenirNombreMesuresMoyennesInvalides ()) ;
//----- Afficher la RAM libre
  setLineForTextSize (7, 2) ;
  setColumnForTextSize (1, 2) ;
  tft.setTextColor (TFT_WHITE, TFT_BLACK);    
  tft.printf ("RAM libre %6u octets", minFreeHeap ()) ;
}

//----------------------------------------------------------------------------------------------------------------------
