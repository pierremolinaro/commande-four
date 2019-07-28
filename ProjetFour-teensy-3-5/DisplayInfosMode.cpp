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
  tft.setTextColor (ILI9341_WHITE, ILI9341_BLACK);
  String s = "Temp" LOWERCASE_E_ACUTE "rature : " ;
  s += String (getSensorTemperature ()) ;
  s += " " DEGREE_CHAR "C" ;
  printWithPadding (s.c_str (), 24) ;
//----- Température brute
  setLineColumnForTextSize (5, 1, 2) ;
  s = "Brute : " ;
  s += String (getRawTemperature ()) ;
  s += " " DEGREE_CHAR "C" ;
  printWithPadding (s.c_str (), 24) ;
//----- Température jonction
  setLineColumnForTextSize (6, 1, 2) ;
  s = "Jonction : " ;
  s += String (getJunctionTemperature ()) ;
  s += " " DEGREE_CHAR "C" ;
  printWithPadding (s.c_str (), 24) ;
//--- Nombre de mesures incorrectes
  setLineColumnForTextSize (7, 1, 2) ;
  afficherCompteurErreurs ("Mesures invalides : ", getFaultlySampleCount ()) ;
//--- Nombre de mesures rejetées
  setLineColumnForTextSize (8, 1, 2) ;
  afficherCompteurErreurs ("Mesures rejet" LOWERCASE_E_ACUTE "es : ", getRejectedInconsistentSampleCount ()) ;
//--- Nombre de moyennes invalides
  setLineColumnForTextSize (9, 1, 2) ;
  afficherCompteurErreurs ("Moyennes invalides : ", obtenirNombreMesuresMoyennesInvalides ()) ;
//----- Afficher la RAM libre
  setLineColumnForTextSize (10, 1, 2) ;
  tft.setTextColor (ILI9341_WHITE, ILI9341_BLACK);
  tft.printf ("RAM libre %6u octets", minFreeHeap ()) ;
}

//----------------------------------------------------------------------------------------------------------------------
