/************************************************************************************
Project by Antoine COLSON and Clément NADER
directed by Pierre MOLINARO and Jean-Claude BARDIAUX

This sketch is used to control an oven in order to follow a temperature graph 

#####################################################################################
############################### A PROJECT FROM ECN  #################################
#####################################################################################

************************************************************************************/

//--- Type de carte : MH ET LIVE ESP32MiniKit

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Libraries
//    - TFT_eSPI
//    - Rtc_by_Makuna
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "Defines.h"
#include "RotaryEncoder.h"
#include "TFT.h"
#include "RealTimeClock.h"
#include "TemperatureSensor.h"
#include "SD_Card.h"
#include "Backlight.h"
#include "ManualMode.h"
#include "OvenControl.h"
#include "LogData.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Menu and submenu setting declarations
static uint16_t gMode   = 0 ; // This is which menu mode we are in at any given time (top level or one of the submenus)
static uint16_t nbMenus = 5 ; // This is the number of submenus of the mode we are in

// In order to set time
static uint16_t settingYear   = 2000;
static uint8_t  settingMonth  = 1;
static uint8_t  settingDay    = 1;
static uint8_t  settingHour   = 0;
static uint8_t  settingMinute = 0;

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------

static bool leapYear(uint16_t year) {
  return(((year%4 == 0) && (year%100 != 0)) || (year%400 == 0));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------

// The different curve patterns of temperature
static String  gFileNameArray [maxnbCurves]; // to stock the names of the curves in the SD card
static uint8_t nbCurves          = 0;
static String  nameOfCurve       = "";
static String  infoNameOfCurve   = "";
static uint8_t numPage           = 0;

// Declarations for the functionment of the oven
static bool     isRunning       = false;
static bool     isDelayed       = false;
static uint16_t launchDelay     = 0;
static uint32_t t_delayed_start = 0; // the number of minutes since the 1st January 2000 of the time when we will begin the cycle

static uint32_t t_of_launching    = 0; // the number of minutes since the 1st January 2000
static uint32_t t_since_launching = 0; // the number of minutes since the launching 
static uint16_t tmax              = 0; // the number of minuts of the program selected

static       double wantedTemp   = 0;     // the temperature wanted at this moment 
static const double tolTemp      = 1.;    // tolerance of temperature 
static       bool   increaseTemp = false; // launch the resistance of the oven if oven is not enough hot

// Declarations to create a new curve
static TimeTemp arrayTimeTemp[22];
static uint8_t  len         = 0;
static String   displayName = "";
static uint8_t  MAJminOth   = 0; // 0 -> MAJ, 1 -> min, 2 -> Others

// Declarations when a cycle ends
static uint64_t delayBuzz = 0;

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
//                                    SETUP
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup (void) {
// ----------DEBUGGING section of setup----------
  Serial.begin (115200) ;     // DEBUGGING: opens serial port, sets data rate to 115200 bps
// ----------LEDs section of setup----------
  pinMode (LED_FOUR_CHAUD, OUTPUT) ;
//--- Backlight
  initBacklight () ;
// ----------Buzzer section of setup----------
// attach the channel to the buzzer to be controlled
  ledcAttachPin (BUZZER_PIN, CANAL_PWM_BUZZER) ;
// configure PWM functionalitites
  ledcSetup (CANAL_PWM_BUZZER, FREQUENCE_BUZZER, RESOLUTION_PWM_BUZZER);
//--- Initializations
  initTemperatureSensor () ;
  initEncoder () ;
  initRealTimeClock () ;
  initOvenControl () ;
// ----------SD card reader section of setup----------
  initSDcard () ;
  nbCurves = numberFiles(valuesDir);
  getDisplayNames(gFileNameArray);
// ----------TFT screen section of setup----------
  initScreen () ;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
//                                    LOOP
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------

static uint32_t delayScreen = 2000;
static uint32_t gInstantAffichagePiedPage = 2000 ;
static uint32_t delaySD     = 0;

void loop (void) {
//--- Retro-eclairage
  updateBacklight () ;
// ----------Updating the time----------
  updateTime () ;
// ----------Updating the temperature----------
  updateTemp () ;

  writeLogFile () ;
// ----------Updating the state of the LEDs----------
// Light on the LED 1 if the oven is hot
  digitalWrite (LED_FOUR_CHAUD, getTemp () > 200.0) ;
// Light on the LED 2 if a process is running
//  digitalWrite (LED_EN_MARCHE, isRunning) ;
// ----------Changing Mode----------
    if (clickPressed ()) {
        rotaryMenu(); // change the mode in function of which mode we are in and the position of the rotary encoder
        nbSubMenus(); // update the number of submenus of the mode
    }
    // ----------The start of the program has been set with a delay----------
    if (isDelayed) {
        if (getMinutesSince2000() >= t_delayed_start) {
            isDelayed = false;
            isRunning = true;
            delaySD = millis() + 1000;
            t_of_launching = getMinutesSince2000 ();
            wantedTemp = giveTemp(nameOfCurve, t_since_launching); // obtain the wanted temperature according to the duration with SD card info
        }
    }
    // ----------The oven is running----------
    if (isRunning) {
        t_since_launching = getMinutesSince2000() - t_of_launching;
        if (t_since_launching > tmax) { // the cycle ends
            gMode = 5;
            clearScreen();
            delayBuzz = millis();
            isRunning = false;
            increaseTemp = false;
            digitalWrite(PIN_OVEN_RELAY, LOW);
        }
    }
    if (isRunning) {
        if (millis() > delaySD) { // reading every minute
            delaySD += 60000;
            wantedTemp = giveTemp(nameOfCurve, t_since_launching); // obtain the wanted temperature according to the duration with SD card info
        }
        // ----------Taking decision concerning the using of relay----------
        // If the current temp is in the tolerance of the wanted temperature, we don't change the bool 
        // If the current temperature is too low, oven heats
        if (getTemp() < (wantedTemp - tolTemp)) { 
            increaseTemp = true; 
        }
        // If the current temperature is too high, oven stops
        if (getTemp() > (wantedTemp + tolTemp)) {
            increaseTemp = false;
        }
        // Adapting or not the activity of relay
        digitalWrite (PIN_OVEN_RELAY, increaseTemp);
    }
    // ----------A cycle ends----------
    if (gMode == 5) { // buzz and blink during one second every 5 seconds
        for (uint16_t deltaDelay = 0; deltaDelay < 5000; deltaDelay += 500) {
            if (millis() > delayBuzz + deltaDelay) {
                if (deltaDelay == 0) {
                    ledcWrite(CANAL_PWM_BUZZER, 128);
                    digitalWrite(LED_EN_MARCHE, HIGH);
                } else if (deltaDelay == 1000) {
                    ledcWrite (CANAL_PWM_BUZZER, 0);
                    digitalWrite(LED_EN_MARCHE, !digitalRead(LED_EN_MARCHE));
                } else if (deltaDelay == 4500) {
                    delayBuzz += 5000;
                    digitalWrite(LED_EN_MARCHE, !digitalRead(LED_EN_MARCHE));
                } else {
                    digitalWrite(LED_EN_MARCHE, !digitalRead(LED_EN_MARCHE));
                }
            }
        }
    } else if (delayBuzz != 0 && gMode == 0) { // stop buzzing and blinking when we click, and clear the screen
        delayBuzz = 0;
        ledcWrite (CANAL_PWM_BUZZER, 0) ;
        digitalWrite (LED_EN_MARCHE, LOW) ;
        clearScreen () ;
        clearPrintPermanent () ;
    }
    // ----------Delayed Start Mode----------
    if (gMode == 13) {
        launchDelay = 10*useRotaryEncoder(0, 23*6+5); // we increment with a step of 10mn and from 0 to 23h50mn
    }
    // ----------Changing Delay Mode----------
    else if (gMode == 15) {
        launchDelay = 10*useRotaryEncoder(0, 23*6+5); // we increment with a step of 10mn and from 0 to 23h50mn
    }
    // ----------Setting Time Modes----------
    else if (gMode == 30) {
        settingHour = useRotaryEncoder(0, 23);

    } else if (gMode == 31) {
        settingMinute = useRotaryEncoder(0, 59);
    } else if (gMode == 32) {
        settingYear = useRotaryEncoder(2000, 3000);
    } else if (gMode == 33) {
        settingMonth = useRotaryEncoder(1, 12);
    } else if (gMode == 34) {
        uint8_t monthDays[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        if (leapYear(currentYear())) monthDays[1] = 29;
        settingDay = useRotaryEncoder(1, monthDays[currentMonth()-1]);
    }
    // ----------Selecting Time Mode----------
    else if (gMode == 440) {
        // we increment with a step of 10mn and from the last time up to 1000mn
        arrayTimeTemp[len-1].Time = 10*useRotaryEncoder((uint16_t)arrayTimeTemp[len-2].Time/10 + 1, 100);
    }
    // ----------Selecting Temperature Mode----------
    else if (gMode == 441) {
        // we increment with a step of 10°C and from 0 to 1100°C
        arrayTimeTemp[len-1].Temp = 10*useRotaryEncoder(0, 110);
    }
 //--- Affichage du pied de page toutes les secondes
   if (millis () > gInstantAffichagePiedPage) {
     gInstantAffichagePiedPage += 1000 ;
     const uint16_t timeLeft = tmax - t_since_launching;
     const uint16_t timeBeforeStart = t_delayed_start - getMinutesSince2000();
     printPermanent (currentYear(), currentMonth(), currentDay(), currentHour(), currentMinute(),
                     currentSecond(), isRunning, timeLeft, isDelayed, timeBeforeStart);
   }
    // ----------Printing on the TFT screen----------
    if (millis() > delayScreen) { // printing every 100 ms
        delayScreen += 100;        
        // ----------Printing the current menu----------
        if (gMode == 0) {
            printMainMenu (encoderPosition(nbMenus), isRunning, isDelayed);
        }else if (gMode == 999) { // Mode manuel
           imprimerEcranModeManuel (encoderPosition(nbMenus)) ;
        }else if (gMode == 998) {
           reglageConsigneModeManuel () ;
           imprimerEcranModeManuel (encoderPosition(nbMenus)) ;
        }else if (gMode == 1) {
            printSelectCurveMenu (encoderPosition (nbMenus), nbCurves, gFileNameArray, numPage);
        } else if (gMode == 10) {
            printShowValuesMenu (encoderPosition(nbMenus));
        } else if (gMode == 11) {
            printPlotGraphMenu (encoderPosition(nbMenus));
        } else if (gMode == 12) {
            printStartOrDelayMenu (encoderPosition(nbMenus), tmax, currentHour(), currentMinute());
        } else if (gMode == 13) {
            printDelayMenu (launchDelay, tmax, currentHour(), currentMinute());
        } else if (gMode == 14) {
            printStopMenu (encoderPosition(nbMenus));
        } else if (gMode == 15) {
            printChangeDelayMenu (launchDelay, tmax, currentHour(), currentMinute());
        } else if (gMode == 2) {
            const uint16_t timeLeft = tmax - t_since_launching;
            printInfoMenu (wantedTemp, isRunning, timeLeft, increaseTemp);
        } else if (gMode == 3) {
            printSetTimeMenu (encoderPosition(nbMenus), currentYear(), currentMonth(), currentDay(), currentHour(), currentMinute());
        } else if (gMode == 30) {
            printSetTimeSubMenu (gMode, currentYear(), currentMonth(), currentDay(), settingHour, currentMinute());
        } else if (gMode == 31) {
            printSetTimeSubMenu (gMode, currentYear(), currentMonth(), currentDay(), currentHour(), settingMinute);
        } else if (gMode == 32) {
            printSetTimeSubMenu (gMode, settingYear, currentMonth(), currentDay(), currentHour(), currentMinute());
        } else if (gMode == 33) {
            printSetTimeSubMenu(gMode, currentYear(), settingMonth, currentDay(), currentHour(), currentMinute());
        } else if (gMode == 34) {
            printSetTimeSubMenu(gMode, currentYear(), currentMonth(), settingDay, currentHour(), currentMinute());
        } else if (gMode == 4) {
            printManageCurvesMenu(encoderPosition(nbMenus));
        } else if (gMode == 40) {
            printSelectCurveMenu(encoderPosition(nbMenus), nbCurves, gFileNameArray, numPage);
        } else if (gMode == 400) {
            printMaxNbCurvesErrorMenu();
        } else if (gMode == 41) {
            printShowValuesMenu(encoderPosition(nbMenus));
        } else if (gMode == 42) {
            printPlotGraphMenu(encoderPosition(nbMenus));
        } else if (gMode == 43) {
            printChooseNameMenu(encoderPosition(nbMenus), displayName, MAJminOth);
        } else if (gMode == 430) {
            printNameContinueMenu(encoderPosition(nbMenus), displayName);
        } else if (gMode == 431 || gMode == 461) {
            printNameErrorMenu(nameOfCurve, gMode);
        } else if (gMode == 44) {
            printSelectValuesMenu(encoderPosition(nbMenus), arrayTimeTemp, len);
        } else if (gMode == 440 || gMode == 441) {
            printSelectTimeTempMenu(arrayTimeTemp, len, gMode);
        } else if (gMode == 45) {
            printPlotGraphMenu(encoderPosition(nbMenus), arrayTimeTemp, len);
        } else if (gMode == 46) {
            printSelectCurveMenu(encoderPosition(nbMenus), nbCurves, gFileNameArray, numPage);
        } else if (gMode == 47) {
            printDeleteCurveMenu(encoderPosition(nbMenus), infoNameOfCurve);
        } else if (gMode == 5) {
            printEndCycleMenu();
        }
    }
}

/*====================================================================================*
 *                                OTHER FUNCTIONS                                     *
 *====================================================================================*/
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
// To update the number of submenus when the mode changes
void nbSubMenus(void) {
    switch (gMode) {
        case 0  : nbMenus = 5 ;
                  break;
        case 999  : nbMenus = 3 ;
                   break;
        case 998  : nbMenus = 1 ;
                   break;
        case 1  : if (numPage > 0 && nbCurves <= 6*(numPage+1)) { // last page
                      nbMenus = 2 + nbCurves - 6*numPage;
                  } else {
                      nbMenus = min(8, 1 + nbCurves - 6*numPage);
                  }
                  break;
        case 10 : nbMenus = 2;
                  break;
        case 11 : nbMenus = 2;
                  break;
        case 12 : nbMenus = 3;
                  break;
        case 14 : nbMenus = 2;
                  break;
        case 15 : nbMenus = 2;
                  break;
        case 3  : nbMenus = 3;
                  break;
        case 4  : nbMenus = 4;
                  break;
        case 40 : if (numPage > 0 && nbCurves <= 6*(numPage+1)) { // last page
                      nbMenus = 2 + nbCurves - 6*numPage;
                  } else {
                      nbMenus = min(8, 1 + nbCurves - 6*numPage);
                  }
                  break;
        case 41 : nbMenus = 2;
                  break;
        case 42 : nbMenus = 2;
                  break;
        case 43 : if (MAJminOth == 2) {
                      nbMenus = 11; // the ten digits from 0 to 9, and _(underscore)
                  } else {
                      nbMenus = 26; // the 26 characters from a to z (either in min or MAJ)
                  }
                  if (displayName.length() == 0) {
                      nbMenus += 2; // we cannot delete a character nor validate
                  } else if (displayName.length() == maxlength) {
                      nbMenus = 3; // we can only delete a character or validate or cancel
                  } else {
                      nbMenus += 4; // we can change from MAJ to min, min to Other, Other to MAJ or delete a character or validate or cancel
                  }
                  break;
        case 430: nbMenus = 2;
                  break;
        case 44 : if (len < 2) { // minimal lenghth we can't delete a column anymore nor validate
                      nbMenus = 3;
                  } else if (len < 22 && arrayTimeTemp[len-1].Time < 1000) {
                      nbMenus = 4;
                  } else { // maximal lenghth we can't add a column anymore
                      nbMenus = 3;
                  }
                  break;
        case 45 : nbMenus = 2;
                  break;
        case 46 : if (numPage > 0 && nbCurves <= 6*(numPage+1)) { // last page
                      nbMenus = 2 + nbCurves - 6*numPage;
                  } else {
                      nbMenus = min(8, 1 + nbCurves - 6*numPage);
                  }
                  break;
        case 47 : nbMenus = 2;
                  break;
        default : nbMenus = 1;
    }
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
// To go from a mode to another
void rotaryMenu() {
    bool doClearScreen = true;
    bool doResetEncoderPos = true;
    
    // ----------Main menu section----------
    if (gMode == 0) {
        switch (encoderPosition(nbMenus)) {
            case 1  : gMode = 2; // to go to information menu
                      break;
            case 2  : gMode = 3; // to go to set time menu
                      break;
            case 3  : gMode = 4; // to go to curves managing menu
                      break;
            case 4  : gMode = 999 ; // Aller en mode manuel
                      entreeModeManuel () ;
                      break;
            case 0 : if (isRunning) {
                          gMode = 14; // to go to stop menu
                      } else if (isDelayed) {
                          gMode = 15; // to go to change delay menu
                          launchDelay = t_delayed_start - getMinutesSince2000();
                          setEncoderPosition((uint16_t)launchDelay/10);
                          doResetEncoderPos = false;
                      } else {
                          gMode = 1; // to go to Select curves menu
                          numPage = 0;
                      }
            default : break ;
        }
    }
    //---- Mode manuel
    else if (gMode == 999) {
      bool revenirPagePrincipale = false ;
      bool saisirConsigne = false ;
      actionModeManuel (encoderPosition(nbMenus), revenirPagePrincipale, saisirConsigne) ;
      if (revenirPagePrincipale) {
        gMode = 0 ;
      }else if (saisirConsigne) {
        gMode = 998 ;
        doResetEncoderPos = false ;
        doClearScreen = false ;
      }else{
        doResetEncoderPos = false ;
        doClearScreen = false ;
      }
    }
    //--- Reglage consigne mode manuel
    else if (gMode == 998) {
      quitterModeReglageConsigneModeManuel () ;
      gMode = 999 ;
      doResetEncoderPos = false;
   }
    // ----------Select curves menu section----------
    else if (gMode == 1) {
        if (numPage > 0 && nbCurves <= 6*(numPage+1)) { // last page
            nbMenus = 2 + nbCurves - 6*numPage;
        } else {
            nbMenus = min(8, 1 + nbCurves - 6*numPage);
        }
        if (encoderPosition(nbMenus) < min(6, nbCurves-6*numPage)) { // selecting the curve, we print 6 curves per page
            gMode = 10;
            nameOfCurve = gFileNameArray[6*numPage + encoderPosition(nbMenus)];
            doClearScreen = false;
            clearScreen();
            printValues(nameOfCurve);
        } else if (nbCurves-6*numPage > 6 && encoderPosition(nbMenus) == 6) { // Next page button
            numPage ++;
        } else if (numPage > 0 && encoderPosition(nbMenus) == nbCurves - 6*numPage) { // Next page button
            numPage = 0; // To come back to the first page
        } else {
            gMode = 0; // to return to main menu
        }
    }
    // ----------Show values menu section----------
    else if (gMode == 10) {
        switch (encoderPosition(nbMenus)) {
            case 0  : gMode = 11;
                      doClearScreen = false;
                      clearScreen();
                      printGraph(nameOfCurve);
                      break;
            default : gMode = 1;
        }
    }
    // ----------Plot graph menu section----------
    else if (gMode == 11) {
        switch (encoderPosition(nbMenus)) {
            case 0  : gMode = 12;
                      tmax = timeMax(nameOfCurve);
                      break;
            default : gMode = 10;
                      doClearScreen = false;
                      clearScreen();
                      printValues(nameOfCurve);
        }
    }
    // ----------Start now or delay menu section----------
    else if (gMode == 12) {
        switch (encoderPosition(nbMenus)) {
            case 0  : gMode = 0; // to return to main menu
                      isRunning = true;
                      delaySD = millis() + 1000;
                      t_of_launching = getMinutesSince2000(); // time when we launch firing
                      wantedTemp = giveTemp(nameOfCurve, t_since_launching); // obtain the wanted temperature according to the duration with SD card info
                      break;
            case 1  : gMode = 13; // to select the value of the delay
                      setEncoderPosition((uint16_t)launchDelay/10);
                      break;
            default : gMode = 11;
                      doClearScreen = false;
                      clearScreen();
                      printGraph(nameOfCurve);
        }
    }
    // ----------Selecting the value of the delay menu section----------
    else if (gMode == 13) {
        gMode = 0; // to return to main menu
        isDelayed = true;
        t_delayed_start = getMinutesSince2000() + launchDelay; // time when we will launch firing
    }
    // ----------Stop cycle menu section----------
    else if (gMode == 14) {
        switch (encoderPosition(nbMenus)) {
            case 0  : gMode = 0; // to return to main menu
                      isRunning = false;
                      increaseTemp = false;
                      digitalWrite (PIN_OVEN_RELAY, increaseTemp);
                      clearPrintPermanent();
                      break;
            default : gMode = 0; // to return to main menu
        }
    }
    // ----------Changing the value of the delay menu section----------
    else if (gMode == 15) {
        gMode = 0; // to return to main menu
        t_delayed_start = getMinutesSince2000() + launchDelay; // time when we will launch firing
        clearPrintPermanent();
    }
    // ----------Set Time menu section----------
    else if (gMode == 3) {
        switch (encoderPosition(nbMenus)) {
            case 0  : gMode = 30; // to go to the setting of the hour
                      setEncoderPosition(currentHour());
                      doResetEncoderPos = false;
                      break;
            case 1  : gMode = 32; // to go to the setting of the year
                      setEncoderPosition(currentYear());
                      doResetEncoderPos = false;
                      break;
            default : gMode = 0; // to return to main menu
        }
    }
    // ----------Set Hour menu section----------
    else if (gMode == 30) {
        doResetEncoderPos = false;
        gMode = 31; // to go to the setting of the minute
        uint32_t tleft = 0;
        uint32_t delayleft = 0;
        if (isRunning) tleft     = getMinutesSince2000() - t_of_launching;
        if (isDelayed) delayleft = t_delayed_start - getMinutesSince2000();
        setHour(settingHour);
        updateTime();
        if (isRunning) t_of_launching  = getMinutesSince2000() - tleft;
        if (isDelayed) t_delayed_start = getMinutesSince2000() + delayleft;
        setEncoderPosition(currentMinute());
    }
    // ----------Set Minute menu section----------
    else if (gMode == 31) {
        gMode = 3; // to return to set time menu
        uint32_t tleft = 0;
        uint32_t delayleft = 0;
        if (isRunning) tleft     = getMinutesSince2000() - t_of_launching;
        if (isDelayed) delayleft = t_delayed_start - getMinutesSince2000();
        setMinute(settingMinute);
        updateTime();
        if (isRunning) t_of_launching  = getMinutesSince2000() - tleft;
        if (isDelayed) t_delayed_start = getMinutesSince2000() + delayleft;
        resetSecond(); // we reset the number of seconds to zero
    }
    // ----------Set Year menu section----------
    else if (gMode == 32) {
        doResetEncoderPos = false;
        gMode = 33; // to go to the setting of the month
        uint32_t tleft = 0;
        uint32_t delayleft = 0;
        if (isRunning) tleft     = getMinutesSince2000() - t_of_launching;
        if (isDelayed) delayleft = t_delayed_start - getMinutesSince2000();
        setYear(settingYear);
        updateTime();
        if (isRunning) t_of_launching  = getMinutesSince2000() - tleft;
        if (isDelayed) t_delayed_start = getMinutesSince2000() + delayleft;
        setEncoderPosition(currentMonth());
    }
    // ----------Set Month menu section----------
    else if (gMode == 33) {
        doResetEncoderPos = false;
        gMode = 34; // to go to the setting of the day
        uint32_t tleft = 0;
        uint32_t delayleft = 0;
        if (isRunning) tleft     = getMinutesSince2000() - t_of_launching;
        if (isDelayed) delayleft = t_delayed_start - getMinutesSince2000();
        setMonth(settingMonth);
        updateTime();
        if (isRunning) t_of_launching  = getMinutesSince2000() - tleft;
        if (isDelayed) t_delayed_start = getMinutesSince2000() + delayleft;
        setEncoderPosition(currentDay());
    }
    // ----------Set Day menu section----------
    else if (gMode == 34) { // the day has to be set in last, as the number of days depends on the month and the year (leap year or not)
        gMode = 3; // to return to set time menu
        uint32_t tleft = 0;
        uint32_t delayleft = 0;
        if (isRunning) tleft     = getMinutesSince2000() - t_of_launching;
        if (isDelayed) delayleft = t_delayed_start - getMinutesSince2000();
        setDay(settingDay);
        updateTime();
        if (isRunning) t_of_launching  = getMinutesSince2000() - tleft;
        if (isDelayed) t_delayed_start = getMinutesSince2000() + delayleft;
    }
    // ----------Curves managing menu section----------
    else if (gMode == 4) {
        switch (encoderPosition(nbMenus)) {
            case 0  : gMode = 40; // to show information about the curves
                      numPage = 0;
                      break;
            case 1  : if (nbCurves < maxnbCurves) {
                          gMode = 43; // to chose the name of the new curve
                          displayName = "";
                          MAJminOth = 0;
                      } else { // we have reached the maximum of curves
                          gMode = 400;
                      }
                      break;
            case 2  : gMode = 46; // to delete a curve
                      numPage = 0;
                      break;
            default : gMode = 0; // to return to main menu
        }
    }
    // ----------Select curves menu section----------
    else if (gMode == 40) {
        if (encoderPosition(nbMenus) < min(6, nbCurves-6*numPage)) { // selecting the curve, we print 6 curves per page
            gMode = 41;
            infoNameOfCurve = gFileNameArray[6*numPage + encoderPosition(nbMenus)];
            doClearScreen = false;
            clearScreen();
            printValues(infoNameOfCurve);
        } else if (nbCurves-6*numPage > 6 && encoderPosition(nbMenus) == 6) { // Next page button
            numPage ++;
        } else if (numPage > 0 && encoderPosition(nbMenus) == nbCurves - 6*numPage) { // Next page button
            numPage = 0; // To come back to the first page
        } else {
            gMode = 4; // to return to curves managing menu
        }
    }
    // ----------Show values menu section----------
    else if (gMode == 41) {
        switch (encoderPosition(nbMenus)) {
            case 0  : gMode = 42;
                      doClearScreen = false;
                      clearScreen();
                      printGraph(infoNameOfCurve);
                      break;
            default : gMode = 40;
        }
    }
    // ----------Plot graph menu section----------
    else if (gMode == 42) {
        switch (encoderPosition(nbMenus)) {
            case 0  : gMode = 4;
                      break;
            default : gMode = 41;
                      doClearScreen = false;
                      clearScreen();
                      printValues(infoNameOfCurve);
        }
    }
    // ----------Selecting name of the new curve menu section----------
    else if (gMode == 43) {
        // If the mode doesn't change, we stay in the actual, ie the mode 43
        doResetEncoderPos = false;
        if (displayName.length() == 0) { // we cannot delete a character nor validate
            if (MAJminOth == 0) {
                if (encoderPosition(nbMenus) == 26) { // we change from MAJ to min, min to Other, Other to MAJ
                    MAJminOth = (MAJminOth+1) % 3;
                    doResetEncoderPos = true;
                } else if (encoderPosition(nbMenus) == 27) { // we cancel
                    gMode = 4;
                } else {
                    displayName += (char) (65 + encoderPosition(nbMenus)); // (char)65 -> A
                    MAJminOth = 1;
                }
            } else if (MAJminOth == 1) {
                if (encoderPosition(nbMenus) == 26) { // we change from MAJ to min, min to Other, Other to MAJ
                    MAJminOth = (MAJminOth+1) % 3;
                    doResetEncoderPos = true;
                } else if (encoderPosition(nbMenus) == 27) { // we cancel
                    gMode = 4;
                } else {
                    displayName += (char) (97 + encoderPosition(nbMenus)); // (char)97 -> a
                }
            } else {
                if (encoderPosition(nbMenus) == 11) { // we change from MAJ to min, min to Other, Other to MAJ
                    MAJminOth = (MAJminOth+1) % 3;
                    doResetEncoderPos = true;
                } else if (encoderPosition(nbMenus) == 12) { // we cancel
                    doResetEncoderPos = true;
                    gMode = 4;
                } else if (encoderPosition(nbMenus) == 10) { // we add an underscore
                    displayName += (char) (95); // (char)95 -> _
                } else {
                    displayName += (char) (48 + encoderPosition(nbMenus)); // (char)48 -> 0
                }
            }
        } else if (displayName.length() == maxlength) { // we can only delete a character or validate
            if (encoderPosition(nbMenus) == 0) { // Validate
                gMode = 441; // to create a new curve
            } else if (encoderPosition(nbMenus) == 1) { // we cancel
                doResetEncoderPos = true;
                gMode = 4;
            } else { // delete the character at the right
                displayName.remove(displayName.length()-1);
                if (MAJminOth == 2) setEncoderPosition(12); // to stay on the delete place
                else setEncoderPosition(27);
            }
        } else { // we can change from MAJ to min, min to Other, Other to MAJ or delete a character or validate
            if (MAJminOth == 0) { /*** MAJ ***/
                if (encoderPosition(nbMenus) == 26) { // we change from MAJ to min, min to Other, Other to MAJ
                    MAJminOth = (MAJminOth+1) % 3;
                    doResetEncoderPos = true;
                } else if (encoderPosition(nbMenus) == 27) { // delete the character at the right
                    displayName.remove(displayName.length()-1);
                    if (displayName.length() == 0) { // if we have delete all the characters
                        doResetEncoderPos = true;
                        MAJminOth = 0;
                    }
                } else if (encoderPosition(nbMenus) == 28) { // Validate
                    gMode = 441; // to create a new curve
                } else if (encoderPosition(nbMenus) == 29) { // we cancel
                    doResetEncoderPos = true;
                    gMode = 4;
                } else {
                    displayName += (char) (65 + encoderPosition(nbMenus)); // (char)65 -> A
                    MAJminOth = 1;
                    if (displayName.length() == maxlength) { // if we have fill in all the places
                        doResetEncoderPos = true;
                    }
                }
            } else if (MAJminOth == 1) { /*** min ***/
                if (encoderPosition(nbMenus) == 26) { // we change from MAJ to min, min to Other, Other to MAJ
                    MAJminOth = (MAJminOth+1) % 3;
                    doResetEncoderPos = true;
                } else if (encoderPosition(nbMenus) == 27) { // delete the character at the right
                    displayName.remove(displayName.length()-1);
                    if (displayName.length() == 0) { // if we have delete all the characters
                        doResetEncoderPos = true;
                        MAJminOth = 0;
                    }
                } else if (encoderPosition(nbMenus) == 28) { // Validate
                    gMode = 441; // to create a new curve
                } else if (encoderPosition(nbMenus) == 29) { // we cancel
                    doResetEncoderPos = true;
                    gMode = 4;
                } else {
                    displayName += (char) (97 + encoderPosition(nbMenus)); // (char)97 -> a
                    if (displayName.length() == maxlength) { // if we have fill in all the places
                        doResetEncoderPos = true;
                    }
                }
            } else { /*** Others ***/
                if (encoderPosition(nbMenus) == 11) { // we change from MAJ to min, min to Other, Other to MAJ
                    MAJminOth = (MAJminOth+1) % 3;
                    doResetEncoderPos = true;
                } else if (encoderPosition(nbMenus) == 12) { // delete the character at the right
                    displayName.remove(displayName.length()-1);
                    if (displayName.length() == 0) { // if we have delete all the characters
                        doResetEncoderPos = true;
                        MAJminOth = 0;
                    }
                } else if (encoderPosition(nbMenus) == 13) { // Validate
                    gMode = 441; // to create a new curve
                } else if (encoderPosition(nbMenus) == 14) { // we cancel
                    gMode = 4;
                    doResetEncoderPos = true;
                } else if (encoderPosition(nbMenus) == 10) { // we add an underscore
                    displayName += (char) (95); // (char)95 -> _
                    if (displayName.length() == maxlength) { // if we have fill in all the places
                        doResetEncoderPos = true;
                    }
                } else {
                    displayName += (char) (48 + encoderPosition(nbMenus)); // (char)48 -> 0
                    if (displayName.length() == maxlength) { // if we have fill in all the places
                        doResetEncoderPos = true;
                    }
                }
            }
        }
        if (gMode == 441) { // we validate the name
            if (isRunning && displayName == nameOfCurve) { // the name is the same as the one of the selected program
                gMode = 431;
            }
            else if (stringInArray(displayName, gFileNameArray, nbCurves)) { // the name is already given
                gMode = 430;
            }
            else {
                len = 1;
                arrayTimeTemp[0].Time = 0;
                setEncoderPosition(0);
                doClearScreen = false;
                clearScreen();
                drawTab();
            }
        }
    }
    // ----------We have reached the maximum of curves----------
    else if (gMode == 400) {
        gMode = 4;
    }
    // ----------The name of the curve is already given, choose between continue or return----------
    else if (gMode == 430) {
        switch (encoderPosition(nbMenus)) {
            case 0  : gMode = 43; // to return back
                      break;
            default : gMode = 441; // to continue
                      setEncoderPosition(0);
                      doClearScreen = false;
                      clearScreen();
                      drawTab();
        }
    }
    // ----------The name of the curve is the one of the running program, we return----------
    else if (gMode == 431) {
        gMode = 43;
    }
    // ----------Choose between add or delete a column, or validate the new curve menu section----------
    else if (gMode == 44) {
        clearPrintSelectValuesMenu();
        if (len < 2) { // minimal lenghth we can't validate
            switch (encoderPosition(nbMenus)) {
                case 0  : gMode = 440; // to create a new column
                          setEncoderPosition((uint16_t)arrayTimeTemp[len-1].Time/10 + 1); // initialization to the past values
                          len ++;
                          drawTab();
                          break;
                case 1  : gMode = 43;
                          break;
                default : gMode = 441; // to delete the last column, we only have one column so, we change only the temperature, the time is equal to 0
                          arrayTimeTemp[0].Time = 0;
                          setEncoderPosition(0);
                          drawTab();
            }
        } else if (len < 22 && arrayTimeTemp[len-1].Time < 1000) {
             switch (encoderPosition(nbMenus)) {
                case 0  : gMode = 440; // to create a new column
                          setEncoderPosition((uint16_t)arrayTimeTemp[len-1].Time/10 + 1); // initialization to the past
                          len ++;
                          drawTab();
                          break;
                case 1  : gMode = 45; // to validate
                          break;
                case 2  : gMode = 43;
                          break;
                default : len --; // to delete the last column
                          clearScreen();
                          printValues(arrayTimeTemp, len);
            }
        } else { // maximal lenghth we can't add a column anymore
            switch (encoderPosition(nbMenus)) {
                case 0  : gMode = 45; // to validate
                          break;
                case 1  : gMode = 43;
                          break;
                default : len --; // to delete the last column
                          clearScreen();
                          printValues(arrayTimeTemp, len);
            }
        }
        if ((gMode != 45) && (gMode != 43)) { // if we have not validated nor returned back
            doClearScreen = false;
        }
        if (gMode == 440 || gMode == 441) {
            doResetEncoderPos = false;
        }
    }
    // ----------Selecting the time menu section----------
    else if (gMode == 440) {
        gMode = 441; // to select the temperature
        setEncoderPosition((uint16_t)arrayTimeTemp[len-2].Temp/10);
        doClearScreen = false;
        doResetEncoderPos = false;
        drawTab();
    }
    // ----------Selecting the temperature menu section----------
    else if (gMode == 441) {
        gMode = 44; // to choose either add or delete a column, or validate
        clearScreen();
        printValues(arrayTimeTemp, len);
        doClearScreen = false;
    }
    // ----------Plotting the corresponding graph of the new curve menu section----------
    else if (gMode == 45) {
        switch (encoderPosition(nbMenus)) {
            case 0  : gMode = 4;
                      clearScreen();
                      printCreationMenu();
                          createAll(displayName, arrayTimeTemp, len);
                      if (! stringInArray(displayName, gFileNameArray, nbCurves)) {
                          nbCurves ++;
                          gFileNameArray[nbCurves-1] = displayName;
                      }
                      break;
            default : gMode = 44; // to return
                      clearScreen();
                      printValues(arrayTimeTemp, len);
                      doClearScreen = false;
        }
    }
    // ----------Select curves menu section----------
    else if (gMode == 46) {
        if (encoderPosition(nbMenus) < min(6, nbCurves-6*numPage)) { // selecting the curve, we print 6 curves per page
            gMode = 47;
            infoNameOfCurve = gFileNameArray[6*numPage + encoderPosition(nbMenus)];
            if (isRunning && infoNameOfCurve == nameOfCurve) { // the name is the same as the one of the selected program
                gMode = 461;
                doResetEncoderPos = false;
            }
        } else if (nbCurves-6*numPage > 6 && encoderPosition(nbMenus) == 6) { // Next page button
            numPage ++;
        } else if (numPage > 0 && encoderPosition(nbMenus) == nbCurves - 6*numPage) { // Next page button
            numPage = 0; // To come back to the first page
        } else {
            gMode = 4; // to return to curves managing menu
        }
    }
    // ----------The name of the curve is the one of the running program, we return----------
    else if (gMode == 461) {
        gMode = 46;
        doResetEncoderPos = false;
    }
    // ----------Delete curve menu section----------
    else if (gMode == 47) {
        switch (encoderPosition(nbMenus)) {
            case 0  : gMode = 46;
                      break;
            default : gMode = 4;
                      deleteCurve(infoNameOfCurve);
                      nbCurves --;
                      getDisplayNames(gFileNameArray);
        }
    }
    else {
        gMode = 0; // to return to main menu
    }
    if (doResetEncoderPos) {
        setEncoderPosition(0); // to reset the encoderPosition to zero
    }
    if (doClearScreen) {
        clearScreen();
    }
    Serial.printf("Mode %u\n", gMode);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
