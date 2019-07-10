#include "AutomaticMode.h"
#include "PrintProgramList.h"
#include "ProgramListMode.h"
#include "ProgramFiles.h"
#include "TFT.h"
#include "SDCard.h"
#include "RotaryEncoder.h"
#include "RealTimeClock.h"
#include "OvenControl.h"
#include "TemperatureSensor.h"
#include "gcc-diagnostics.h"

//----------------------------------------------------------------------------------------------------------------------
//   STATIC VARIABLES
//----------------------------------------------------------------------------------------------------------------------

static uint32_t gSelectedItemIndex ;
static uint32_t gDelayedStartInMinutes ;
static bool gSDCardIsMounted ;

enum class DisplayPhase { list, graph, table, startConfiguration, running } ;

static DisplayPhase gDisplayPhase = DisplayPhase::list ;

static uint32_t gSelectedItemIndexInStartScreenSubMode ;
static bool gEditionInStartScreenSubMode ;

//----------------------------------------------------------------------------------------------------------------------
//   ENTER AUTOMATIC MODE
//----------------------------------------------------------------------------------------------------------------------

void enterAutomaticMode (void) {
  tft.fillScreen (ILI9341_BLACK) ;
  gSDCardIsMounted = false ;
  gDisplayPhase = DisplayPhase::list ;
  gSelectedItemIndex = 0 ;
  gSelectedItemIndexInStartScreenSubMode = 0 ;
  setEncoderRange (0, gSelectedItemIndex, 0, true) ;
}

//----------------------------------------------------------------------------------------------------------------------
//   PRINT START SCREEN
//----------------------------------------------------------------------------------------------------------------------

static void displayAutomaticStartScreen (void) {
// ----------Return----------
  setLineColumnForTextSize (0, 0, 2) ;
  tft.setTextSize (2) ;
  setMenuColor (gSelectedItemIndexInStartScreenSubMode == 0, false) ;
  tft.print ("Abandon") ;
//--- Programme
  tft.setTextColor (ILI9341_WHITE, ILI9341_BLACK);
  tft.printf (" %s", programName ().c_str ()) ;
//--- Durée
  setLineColumnForTextSize (1, 1, 2, true) ;
  const uint32_t programDuration = programDurationInMinutes () ;
  tft.printf ("Dur" LOWERCASE_E_ACUTE "e : %u h %u min", programDuration / 60, programDuration % 60) ;
//--- Démarrage différé
  setLineColumnForTextSize (3, 1, 2) ;
  tft.print ("Diff" LOWERCASE_E_ACUTE "r" LOWERCASE_E_ACUTE " de ") ;
  setMenuColor (gSelectedItemIndexInStartScreenSubMode == 1, gEditionInStartScreenSubMode) ;
  tft.printf ("%02u", gDelayedStartInMinutes / 60) ;
  tft.setTextColor (ILI9341_WHITE, ILI9341_BLACK);
  tft.print (" h ") ;
  setMenuColor (gSelectedItemIndexInStartScreenSubMode == 2, gEditionInStartScreenSubMode) ;
  tft.printf ("%02u", gDelayedStartInMinutes % 60) ;
  tft.setTextColor (ILI9341_WHITE, ILI9341_BLACK);
  tft.print (" min") ;
//--- Démarrage effectif
  const uint32_t start = gDelayedStartInMinutes + currentHour () * 60 + currentMinute () ;
  setLineColumnForTextSize (4, 1, 2, true) ;
  tft.print ("D" LOWERCASE_E_ACUTE "marrage effectif :") ;
  setLineColumnForTextSize (5, 3, 2, true) ;
  uint32_t spaces = 0 ;
  switch (start / 60 / 24) {
  case 0 :
    tft.print ("aujourd'hui") ;
    spaces = 1 ;
    break ;
  case 1 :
    tft.print ("demain") ;
    spaces = 6 ;
    break ;
  default :
    tft.printf ("dans %u jours", start / 60 / 24) ;
    break ;
  }
  tft.printf (" " LOWERCASE_A_ACUTE " %02u:%02u", (start / 60) % 24, start % 60) ;
  for (uint32_t i=0 ; i<spaces ; i++) {
    tft.print (" ") ;
  }
//--- Fin prévue
  const uint32_t endTime = start + programDurationInMinutes () ;
  setLineColumnForTextSize (7, 1, 2) ;
  tft.print ("Fin pr" LOWERCASE_E_ACUTE "vue :") ;
  setLineColumnForTextSize (8, 3, 2) ;
  spaces = 0 ;
  switch (endTime / 60 / 24) {
  case 0 :
    tft.print ("aujourd'hui") ;
    spaces = 1 ;
    break ;
  case 1 :
    tft.print ("demain") ;
    spaces = 6 ;
    break ;
  default :
    tft.printf ("dans %u jours", endTime / 60 / 24) ;
    break ;
  }
  tft.printf (" " LOWERCASE_A_ACUTE " %02u:%02u", (endTime / 60) % 24, endTime % 60) ;
  for (uint32_t i=0 ; i<spaces ; i++) {
    tft.print (" ") ;
  }
//--- Démarrer
  setLineColumnForTextSize (7, 5, 3) ;
  tft.setTextSize (3) ;
  setMenuColor (gSelectedItemIndexInStartScreenSubMode == 3, false) ;
  tft.print ("D" LOWERCASE_E_ACUTE "marrer") ;
}

//----------------------------------------------------------------------------------------------------------------------

static void displayAutomaticEndTime (void) { //--- Fin prévue
  const uint32_t MESSAGE_SIZE = 26 ;
  char message [MESSAGE_SIZE] ;
  const uint32_t runningTime = ovenRunningTime () ;
  const RtcDateTime now = currentDateTime () ;
  const uint32_t remaining = now.Hour () * 60 + now.Minute () + programDurationInMinutes () - runningTime / 60 ;
  setLineColumnForTextSize (7, 1, 2, true) ;
  tft.print ("Fin pr" LOWERCASE_E_ACUTE "vue :") ;
  setLineColumnForTextSize (8, 3, 2, true) ;
  switch (remaining / 60 / 24) {
  case 0 :
    tft.print ("aujourd'hui") ;
    break ;
  case 1 :
    tft.print ("demain") ;
    break ;
  default :
    tft.printf ("dans %u jours", remaining / 60 / 24) ;
    break ;
  }
  snprintf (message, MESSAGE_SIZE, " " LOWERCASE_A_ACUTE " %02lu:%02lu", (remaining / 60) % 24, remaining % 60) ;
  printWithPadding (message, MESSAGE_SIZE) ;
}

//----------------------------------------------------------------------------------------------------------------------
//   PRINT RUNNING SCREEN
//----------------------------------------------------------------------------------------------------------------------

static void displayAutomaticRunningScreen (void) {
  const uint32_t programDurationInSecondes = programDurationInMinutes () * 60 ;
  const uint32_t runningTime = ovenRunningTime () ;
// ----------Return----------
  setLineColumnForTextSize (0, 0, 2) ;
  tft.setTextSize (2) ;
  setMenuColor (true, false) ;
  tft.print ((runningTime >= programDurationInSecondes) ? "Retour" : "Arret") ;
//--- Programme
  tft.setTextColor (ILI9341_WHITE, ILI9341_BLACK);
  tft.printf (" %s", programName ().c_str ()) ;
//--- Start delay
  setLineColumnForTextSize (1, 1, 2, true) ;
  const uint32_t startDelayInSeconds = delayForStartingInSeconds () ;
  const uint32_t MESSAGE_SIZE = 26 ;
  char message [MESSAGE_SIZE] ;
  if (startDelayInSeconds > 0) {
    if (startDelayInSeconds < 60) {
      snprintf (message, MESSAGE_SIZE, "D" LOWERCASE_E_ACUTE "marrage dans %lu s", startDelayInSeconds) ;
    }else if (startDelayInSeconds < 3600) {
      snprintf (message, MESSAGE_SIZE, "D" LOWERCASE_E_ACUTE "marrage dans %lu min %lu s", startDelayInSeconds / 60, startDelayInSeconds % 60) ;
    }else{
      snprintf (message, MESSAGE_SIZE, "D" LOWERCASE_E_ACUTE "marrage dans %lu h %lu min", startDelayInSeconds / 3600, (startDelayInSeconds / 60) % 60) ;
    }
    printWithPadding (message, MESSAGE_SIZE) ;
  //--- Fin prévue
    displayAutomaticEndTime () ;
  }else{ // Running
    if (runningTime < programDurationInSecondes) { // Running
      if (runningTime < 60) {
        snprintf (message, MESSAGE_SIZE, "Dur" LOWERCASE_E_ACUTE "e : %lu s", runningTime) ;
      }else if (runningTime < 3600) {
        snprintf (message, MESSAGE_SIZE, "Dur" LOWERCASE_E_ACUTE "e : %lu min %lu s", runningTime / 60, runningTime % 60) ;
      }else{
        snprintf (message, MESSAGE_SIZE, "Dur" LOWERCASE_E_ACUTE "e : %lu h %lu min %lu s", runningTime / 3600, (runningTime / 60) % 60, runningTime % 60) ;
      }
      printWithPadding (message, MESSAGE_SIZE) ;
      const uint32_t t = programDurationInSecondes - runningTime ;
      if (t < 60) {
        snprintf (message, MESSAGE_SIZE, "Reste : %lu s", t) ;
      }else if (t < 3600) {
        snprintf (message, MESSAGE_SIZE, "Reste : %lu min %lu s", t / 60, t % 60) ;
      }else{
        snprintf (message, MESSAGE_SIZE, "Reste : %lu h %lu min %lu s", t / 3600, (t / 60) % 60, t % 60) ;
      }
      setLineColumnForTextSize (3, 1, 2) ;
      printWithPadding (message, MESSAGE_SIZE) ;
    //--- Reference
      setLineColumnForTextSize (4, 1, 2, true) ;
      snprintf (message, MESSAGE_SIZE, "Consigne : %.2f " DEGREE_CHAR "C", temperatureReference ()) ;
      printWithPadding (message, MESSAGE_SIZE) ;
    //--- Temperature
      setLineColumnForTextSize (6, 1, 2) ;
      snprintf (message, MESSAGE_SIZE, "Temp" LOWERCASE_E_ACUTE "rature : %.1f " DEGREE_CHAR "C", getSensorTemperature ()) ;
      printWithPadding (message, MESSAGE_SIZE) ;
    //--- Fin prévue
      displayAutomaticEndTime () ;
 //--- Leaving time
    }else{ // Completed
      printWithPadding ("Termin" LOWERCASE_E_ACUTE, MESSAGE_SIZE) ;
      setLineColumnForTextSize (3, 1, 2) ;
      printWithPadding ("", MESSAGE_SIZE) ;
      setLineColumnForTextSize (4, 1, 2, true) ;
      printWithPadding ("", MESSAGE_SIZE) ;
      setLineColumnForTextSize (6, 1, 2) ;
      printWithPadding ("", MESSAGE_SIZE) ;
      setLineColumnForTextSize (7, 1, 2, true) ;
      printWithPadding ("", MESSAGE_SIZE) ;
      setLineColumnForTextSize (8, 1, 2, true) ;
      printWithPadding ("", MESSAGE_SIZE) ;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
//   PRINT AUTOMATIC MODE
//----------------------------------------------------------------------------------------------------------------------

void printAutomaticModeScreen (void) {
  switch (gDisplayPhase) {
  case DisplayPhase::list :
    printProgramList (gSelectedItemIndex, "Liste") ;
    break ;
  case DisplayPhase::graph :
    break ;
  case DisplayPhase::table :
    break ;
  case DisplayPhase::startConfiguration :
    displayAutomaticStartScreen () ;
    break ;
  case DisplayPhase::running :
    displayAutomaticRunningScreen () ;
    break ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
//   HANDLE ROTARY ENCODER IN AUTOMATIC MODE
//----------------------------------------------------------------------------------------------------------------------

void handleRotaryEncoderInAutomaticMode (void) {
  if (gDisplayPhase == DisplayPhase::list) {
    gSelectedItemIndex = getEncoderValue () ;
  }else if (gDisplayPhase == DisplayPhase::startConfiguration) {
    if (gEditionInStartScreenSubMode) {
      if (gSelectedItemIndexInStartScreenSubMode == 1) { // Hour
       const uint32_t v = getEncoderValue () ;
        if ((v == 0) && (gDelayedStartInMinutes >= 60)) {
          gDelayedStartInMinutes -= 60 ;
        }else if (v == 2) {
          gDelayedStartInMinutes += 60 ;
        }
        setEncoderRange (0, 1, 2, false) ;
      }else if (gSelectedItemIndexInStartScreenSubMode == 2) { // Minute
        const uint32_t v = getEncoderValue () ;
        if ((v == 0) && (gDelayedStartInMinutes > 0)) {
          gDelayedStartInMinutes -= 1 ;
        }else if (v == 2) {
          gDelayedStartInMinutes += 1 ;
        }
        setEncoderRange (0, 1, 2, false) ;
      }
    }else{
      gSelectedItemIndexInStartScreenSubMode = getEncoderValue () ;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
//   CLICK IN AUTOMATIC MODE
//----------------------------------------------------------------------------------------------------------------------

void clickInAutomaticMode (bool & outReturnToMainMenu) {
  const uint32_t programDurationInSecondes = programDurationInMinutes () * 60 ;
  const uint32_t runningTime = ovenRunningTime () ;
  if (gSelectedItemIndex == 0) {
    outReturnToMainMenu = true ;
  }else{
    tft.fillScreen (ILI9341_BLACK) ;
    switch (gDisplayPhase) {
    case DisplayPhase::list :
      { const String fileName = programFileNameAtIndex (gSelectedItemIndex - 1) ;
        const bool ok = readProgramFile (fileName) ;
        Serial.printf ("Read '%s' --> %d\n", fileName.c_str (), ok) ;
        if (ok) {
          gDisplayPhase = DisplayPhase::graph ;
          plotGraph () ;
        }
      } break ;
    case DisplayPhase::graph :
      printTable ("Suivant") ;
      gDisplayPhase = DisplayPhase::table ;
      break ;
    case DisplayPhase::table :
      gDisplayPhase = DisplayPhase::startConfiguration ;
      gSelectedItemIndexInStartScreenSubMode = 0 ;
      setEncoderRange (0, gSelectedItemIndexInStartScreenSubMode, 3, true) ;
     break ;
    case DisplayPhase::startConfiguration :
      switch (gSelectedItemIndexInStartScreenSubMode) {
      case 1 : // Hour selection
      case 2 : // Minute selection
        gEditionInStartScreenSubMode ^= true ;
        if (gEditionInStartScreenSubMode) {
          setEncoderRange (0, 1, 2, false) ;
        }else{
          setEncoderRange (0, gSelectedItemIndexInStartScreenSubMode, 3, true) ;
        }
        break ;
      case 3 : // Start
        startOvenInAutomaticMode (gDelayedStartInMinutes) ;
        gDisplayPhase = DisplayPhase::running ;
        break ;
      default :
        outReturnToMainMenu = true ;
        break ;
      }
      break ;
    case DisplayPhase::running :
      stopOven () ;
      if (runningTime >= programDurationInSecondes) {
        outReturnToMainMenu = true ;
      }else{
        gDisplayPhase = DisplayPhase::startConfiguration ;
      }
      break ;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
