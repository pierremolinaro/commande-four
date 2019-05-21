#include "DisplayInfosMode.h"
#include "TFT.h"
#include "TemperatureSensor.h"
#include "MinFreeHeap.h"

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void displayInfos (void) {
// ----------Return----------
  setLign (0, 2) ;
  tft.setTextSize (2) ;
  setMenuColor (true) ;
  tft.print ("Retour") ;
// ----------Temperature----------
  setLign (3, 2) ; setColumn (1) ;
  tft.setTextColor (TFT_WHITE, TFT_BLACK);
  tft.printf ("Temp" LOWERCASE_E_ACUTE "rature : %7.2f" DEGREE_CHAR "C", getSensorTemperature ()) ;
//--- Nombre de mesures incorrectes
  setLign (4, 2) ; setColumn (1) ;
  afficherCompteurErreurs ("Mesures invalides : ", getFaultlySampleCount ()) ;
//--- Nombre de mesures rejetées
  setLign (5, 2) ; setColumn (1) ;
  afficherCompteurErreurs ("Mesures rejet" "\x82" "es : ", getRejectedInconsistentSampleCount ()) ;
//--- Nombre de moyennes invalides
  setLign (6, 2) ; setColumn (1) ;
  afficherCompteurErreurs ("Moyennes invalides : ", obtenirNombreMesuresMoyennesInvalides ()) ;
//----- Afficher la RAM libre
  setLign (7, 2) ;
  setColumn (1) ;
  tft.setTextColor (TFT_WHITE, TFT_BLACK);    
  tft.printf ("RAM libre %6u octets", minFreeHeap ()) ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
