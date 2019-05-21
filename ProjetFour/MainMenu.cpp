#include "MainMenu.h"
#include "RotaryEncoder.h"
#include "TFT.h"
#include "RealTimeClock.h"
#include "TemperatureSensor.h"
#include "OvenControl.h"
#include "DisplayInfosMode.h"
#include "ManualMode.h"

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   MODE ENUMERATION
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

enum class Mode { main, manual, displayInfos, timeSetting, handleProfiles, automatic } ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   FORWARD DECLARATIONS
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static void displayMainMenu (void) ;
static void printFooter (void) ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   STATIC VARIABLES
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static uint32_t gMainMenuSelectedIndex ;
static Mode gMode = Mode::main ;
static bool gMainMenuInitialized ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// UPDATE DISPLAY
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static void updateDisplay (void) {
  switch (gMode) {
  case Mode::main :
    displayMainMenu () ;
    break ;
  case Mode::manual :
    printManualModeScreen () ;
    break ;
  case Mode::displayInfos :
    displayInfos () ;
    break ;
  case Mode::timeSetting :
  
    break ;
  case Mode::handleProfiles :
  
    break ;
  case Mode::automatic :
    break ;
  }
  printFooter () ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static void displayMainMenu (void) {
  if (!gMainMenuInitialized) {
    setEncoderRange (0, gMainMenuSelectedIndex, 4, true) ;
    gMainMenuInitialized = true ;
  }

  fixerCurseurDemieLignesPourTaille (0, 3) ;
  tft.setTextSize (3) ;
  setMenuColor (gMainMenuSelectedIndex == 0) ;
  tft.print (" D" LOWERCASE_E_ACUTE "marrer Four ") ;

// ----------Show Information----------
  fixerCurseurDemieLignesPourTaille (3, 3) ;
  setMenuColor (gMainMenuSelectedIndex == 1) ;
  tft.print (" Afficher Infos ");
// ----------Set Time----------
  fixerCurseurDemieLignesPourTaille (6, 3) ;
  setMenuColor (gMainMenuSelectedIndex == 2) ;
  tft.print (" R" LOWERCASE_E_ACUTE "gler Heure ");
// ----------Manage Curves----------
  fixerCurseurDemieLignesPourTaille (9, 3);
  setMenuColor (gMainMenuSelectedIndex == 3) ;
  tft.print (" G" LOWERCASE_E_ACUTE "rer Programmes");
//---------- Mode Manuel
  fixerCurseurDemieLignesPourTaille (12, 3);
  setMenuColor (gMainMenuSelectedIndex == 4) ;
  tft.print (" Mode manuel") ;
 //---
//  tft.setTextSize(1);
//  tft.setTextColor(TFT_WHITE, TFT_BLACK);
//  setLign(18, 1); setColumn(17, 3); tft.print(' ');
//  setLign(19, 1); setColumn(17, 3); tft.print(' ');
//  setLign(20, 1); setColumn(17, 3); tft.print(' ');
//  tft.setTextSize(2); tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static void handleRotaryEncoder (void) {
  if (encoderValueDidChange ()) {
    switch (gMode) {
    case Mode::main :
      gMainMenuSelectedIndex = getEncoderValue () ;
      break ;
    case Mode::manual :
      handleRotaryEncoderInManualMode () ;
      break ;
    case Mode::displayInfos :
    
      break ;
    case Mode::timeSetting :
    
      break ;
    case Mode::handleProfiles :
    
      break ;
    case Mode::automatic :
      break ;
    }
  }
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static void handleEncoderClick (void) {
  if (encoderClickPressed ()) {
    Mode newMode = gMode ;
    switch (gMode) {
    case Mode::main :
      switch (gMainMenuSelectedIndex) {
      case 0 : newMode = Mode::automatic ; break ;
      case 1 : newMode = Mode::displayInfos ; break ;
      case 2 : newMode = Mode::timeSetting ; break ;
      case 3 : newMode = Mode::handleProfiles ; break ;
      case 4 : newMode = Mode::manual ; break ;
      default : break ;
      }
      break ;
    case Mode::manual :
      { bool returnToMainMenu = false ;
        clickInManualMode (returnToMainMenu) ;
        if (returnToMainMenu) {
          newMode = Mode::main ;
        }
      } break ;
    case Mode::displayInfos :
      newMode = Mode::main ;
      break ;
    case Mode::timeSetting :
      newMode = Mode::main ;
      break ;
    case Mode::handleProfiles :
      newMode = Mode::main ;
      break ;
    case Mode::automatic :
      newMode = Mode::main ;
      break ;
    }
    if (gMode != newMode) {
      gMode = newMode ;
      switch (gMode) {
      case Mode::main : gMainMenuInitialized = false ; break ;
      case Mode::automatic :  ; break ;
      case Mode::displayInfos : ; break ;
      case Mode::timeSetting : ; break ;
      case Mode::handleProfiles : ; break ;
      case Mode::manual : enterManualMode () ; break ;
      }
      tft.fillScreen (TFT_BLACK); // black screen
    }
  }
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static void printFooter (void) {
  tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
  tft.setTextSize (2) ;
// ----------Printing the date----------
  setLign (nbLign - 2, 2);
  const RtcDateTime now = currentDateTime () ;
  tft.printf ("%02u/%02u/%04u", now.Day (), now.Month (), now.Year ());
// ----------Printing the time----------
  setLign(nbLign - 1, 2);
  tft.printf ("%02u:%02u:%02u", now.Hour (), now.Minute (), now.Second ()) ;
//---- Printing SDCard
  setLign (nbLign - 2, 2) ;
  setColumn (nbColumn - 6) ;
  switch (sdCardStatus ()) {
  case SDCardStatus::mounted :
    tft.setTextColor (TFT_GREEN, TFT_BLACK) ;
    tft.print ("    SD") ;
    break ;
  case SDCardStatus::noCard :  
    tft.setTextColor (TFT_RED, TFT_BLACK) ;
    tft.print (" No SD") ;
    break ;
  case SDCardStatus::insertedNotMounted :  
    tft.setTextColor (TFT_RED, TFT_BLACK) ;
    tft.print ("Err SD") ;
    break ;
  }
// ----------Printing the temperature----------
  tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
  setLign (nbLign - 1, 2) ;
  setColumn (nbColumn - 6) ;
  const uint32_t codeErreur = temperatureSensorErrorFlags () ;
  if (codeErreur == 0) { // Ok
    tft.printf ("%4ld" DEGREE_CHAR "C", lround (getSensorTemperature ())) ;
  }else{
    tft.setTextColor (TFT_RED, TFT_BLACK) ;
    if ((codeErreur & TEMPERATURE_SENSOR_ERROR_NO_CONNECTION) != 0) {
      tft.print ("Absent") ;
    }else if ((codeErreur & TEMPERATURE_SENSOR_ERROR_SHORT_CIRCUITED_TO_GND) != 0) {
      tft.print ("CC GND") ;
    }else{
      tft.print ("CC Vcc") ;
    }
    tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
  }
// ----------Printing ON/OFF/Delayed----------
  setLign(nbLign - 2, 2); setColumn(nbColumn - 2, 1) ; 
  tft.setTextSize (4) ;
  if (ovenIsRunning ()) {
    tft.setTextColor (TFT_GREEN, TFT_BLACK);
    tft.print("ON ");
  }else{
    tft.setTextColor (TFT_RED, TFT_BLACK);
    tft.print("OFF");
  }
//    if (isRunning) { // it is running
//        tft.setTextSize(2);
//        setLign(nbLign - 2, 2); setColumn(nbColumn - 4, 1); tft.print(" ");
//        setLign(nbLign - 1, 2); setColumn(nbColumn - 4, 1); tft.print(" ");
//        setLign(nbLign - 2, 2); setColumn(nbColumn + 6, 1); tft.print("  ");
//        setLign(nbLign - 1, 2); setColumn(nbColumn + 6, 1); tft.print("  ");
//        tft.setTextColor(TFT_GREEN, TFT_BLACK); tft.setTextSize(4);
//        setLign(nbLign - 2, 2); setColumn(nbColumn - 2, 1);
//        tft.print("ON");
//    }
//    else if (!isDelayed) { // it is not running
//        tft.setTextSize(2);
//        setLign(nbLign - 2, 2); setColumn(nbColumn + 8, 1); tft.print(" ");
//        setLign(nbLign - 1, 2); setColumn(nbColumn + 8, 1); tft.print(" ");
//        tft.setTextColor(TFT_RED, TFT_BLACK); tft.setTextSize(4);
//        setLign(nbLign - 2, 2); setColumn(nbColumn - 4, 1);
//        tft.print("OFF");
//    }
//    else { // it is delayed
//        setLign(nbLign*2 - 4, 1); setColumn(nbColumn - 4, 1); tft.setTextSize(1); tft.print("            ");
//        setLign(nbLign*2 - 1, 1); setColumn(nbColumn - 4, 1); tft.setTextSize(1); tft.print("            ");
//        tft.setTextColor(TFT_ORANGE, TFT_BLACK); tft.setTextSize(2);
//        setLign(nbLign*2 - 3, 1); setColumn(nbColumn - 4, 1);
//        tft.printf("Diff%cr%c", 130, 130); // (char)130 -> é
//    }
//    tft.setTextColor(TFT_WHITE, TFT_BLACK); tft.setTextSize(2);

    // ----------Printing time left if a program is running----------
//    if (isRunning) {
//        setLign(nbLign*2 - 5, 1); setColumn(nbColumn*2 - 12, 1); tft.setTextSize(1);
//        tft.print("    restant:");
//        setLign(nbLign - 2, 2); setColumn(nbColumn - 8); tft.setTextSize(2);
//        tft.printf("%3uh%02umn", timeLeft/60, timeLeft%60);
//    }
//    // ----------Printing delay left if a program is delayed----------
//    else if (isDelayed) {
//        setLign(nbLign*2 - 5, 1); setColumn(nbColumn*2 - 12, 1); tft.setTextSize(1);
//        tft.printf("avant d%cbut:", 130); // (char)130 -> é
//        setLign(nbLign - 2, 2); setColumn(nbColumn - 8); tft.setTextSize(2);
//        tft.printf("%3uh%02umn", timeBeforeStart/60, timeBeforeStart%60);
//    } 
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void updateUserInterface (void) {
 // initializeMainMenu () ;
  updateDisplay () ;
  handleRotaryEncoder () ;
  handleEncoderClick () ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
