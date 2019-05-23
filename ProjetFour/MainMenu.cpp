#include "MainMenu.h"
#include "RotaryEncoder.h"
#include "TFT.h"
#include "RealTimeClock.h"
#include "TemperatureSensor.h"
#include "OvenControl.h"
#include "DisplayInfosMode.h"
#include "ManualMode.h"
#include "TimeSettingMode.h"
#include "ProgramMode.h"

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   MODE ENUMERATION
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

enum class Mode { main, manual, displayInfos, timeSetting, program, automatic } ;

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
    printTimeSettingModeScreen () ;
    break ;
  case Mode::program :
    printProgramModeScreen () ;  
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
  setMenuColor (gMainMenuSelectedIndex == 0, false) ;
  tft.print (" D" LOWERCASE_E_ACUTE "marrer Four ") ;

// ----------Show Information----------
  fixerCurseurDemieLignesPourTaille (3, 3) ;
  setMenuColor (gMainMenuSelectedIndex == 1, false) ;
  tft.print (" Afficher Infos ");
// ----------Set Time----------
  fixerCurseurDemieLignesPourTaille (6, 3) ;
  setMenuColor (gMainMenuSelectedIndex == 2, false) ;
  tft.print (" R" LOWERCASE_E_ACUTE "gler Heure ");
// ----------Manage Curves----------
  fixerCurseurDemieLignesPourTaille (9, 3);
  setMenuColor (gMainMenuSelectedIndex == 3, false) ;
  tft.print (" G" LOWERCASE_E_ACUTE "rer Programmes");
//---------- Mode Manuel
  fixerCurseurDemieLignesPourTaille (12, 3);
  setMenuColor (gMainMenuSelectedIndex == 4, false) ;
  tft.print (" Mode manuel") ;
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
      handleRotaryEncoderInTimeSettingMode () ;
      break ;
    case Mode::program :
      handleRotaryEncoderInProgramMode () ;  
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
    bool returnToMainMenu = false ;
    switch (gMode) {
    case Mode::main :
      switch (gMainMenuSelectedIndex) {
      case 0 : newMode = Mode::automatic ; break ;
      case 1 : newMode = Mode::displayInfos ; break ;
      case 2 : newMode = Mode::timeSetting ; break ;
      case 3 : newMode = Mode::program ; break ;
      case 4 : newMode = Mode::manual ; break ;
      default : break ;
      }
      break ;
    case Mode::manual :
      clickInManualMode (returnToMainMenu) ;
      break ;
    case Mode::displayInfos :
      newMode = Mode::main ;
      break ;
    case Mode::timeSetting :
      clickInTimeSettingMode (returnToMainMenu) ;
      break ;
    case Mode::program :
      clickInProgramMode (returnToMainMenu) ;
      break ;
    case Mode::automatic :
      newMode = Mode::main ;
      break ;
    }
    if (returnToMainMenu) {
      newMode = Mode::main ;
    }
    if (gMode != newMode) {
      gMode = newMode ;
      switch (gMode) {
      case Mode::main           : gMainMenuInitialized = false ; break ;
      case Mode::automatic      :  ; break ;
      case Mode::displayInfos   : ; break ;
      case Mode::timeSetting    : enterTimeSettingMode () ; break ;
      case Mode::program : enterProgramMode () ; break ;
      case Mode::manual         : enterManualMode () ; break ;
      }
      tft.fillScreen (TFT_BLACK) ;
    }
  }
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static void printFooter (void) {
  tft.setTextColor (TFT_WHITE, TFT_BLACK) ;
  tft.setTextSize (2) ;
// ----------Printing the date----------
  setLineForTextSize (nbLign - 2, 2);
  const RtcDateTime now = currentDateTime () ;
  tft.printf ("%02u/%02u/%04u", now.Day (), now.Month (), now.Year ());
// ----------Printing the time----------
  setLineForTextSize (nbLign - 1, 2);
  tft.printf ("%02u:%02u:%02u", now.Hour (), now.Minute (), now.Second ()) ;
//---- Printing SDCard
  setLineForTextSize (nbLign - 2, 2) ;
  setColumnForTextSize (nbColumn - 6) ;
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
  setLineForTextSize (nbLign - 1, 2) ;
  setColumnForTextSize (nbColumn - 6) ;
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
  setLineForTextSize (nbLign - 2, 2); setColumnForTextSize (nbColumn - 2, 1) ; 
  tft.setTextSize (4) ;
  if (ovenIsRunning ()) {
    tft.setTextColor (TFT_GREEN, TFT_BLACK);
    tft.print("ON ");
  }else{
    tft.setTextColor (TFT_RED, TFT_BLACK);
    tft.print("OFF");
  }
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void updateUserInterface (void) {
  updateDisplay () ;
  handleRotaryEncoder () ;
  handleEncoderClick () ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
