// ----------Always begin by this pragma----------
#pragma once

// ----------Libraries----------
#include <SPI.h>
#include <TFT_eSPI.h>

// ----------Headers----------
#include "Defines.h"
#include "SD_Card.h"

// ----------Constants declaration----------
const uint8_t nbLign = 15; // with a text size of 2
const uint8_t nbColumn = 26; // with a text size of 2

// ----------Functions declaration----------
/*====================================================================================*
 *                                  initScreen                                        *
 *====================================================================================*
 * This function initialized the TFT screen and sets the rotation in landscape,
 * and draws the logo of Centrale Nantes.
 */
void initScreen(void);

/*====================================================================================*
 *                                   drawBmp                                          *
 *====================================================================================*
 * This function prints on the screen a .bmp image from the SD card.
 */
void drawBmp(String filename);

/*====================================================================================*
 *                               read16 & read32                                      *
 *====================================================================================*
 * These functions read 16- and 32-bit types from the SD card file,
 * they are used for the drawBMP function.
 * BMP data is stored little-endian, Arduino is little-endian too.
 * May need to reverse subscript order if porting elsewhere.
 */
uint16_t read16(fs::File &f);
uint32_t read32(fs::File &f);

/*====================================================================================*
 *                                 clearScreen                                        *
 *====================================================================================*
 * This function clears all the screen except the bottom where are printed the
 * permanent information.
 */
void clearScreen(void);

/*====================================================================================*
 *                                   setLign                                          *
 *====================================================================================*
 * This function sets the cursor at the given lign and at the first column in order
 * to write text in the given text size.
 * The default text size is 2.
 */
void setLign(uint8_t lign, uint8_t textSize = 2);

/*====================================================================================*
 *                                  setColumn                                         *
 *====================================================================================*
 * This function sets the cursor at the same lign and at the given column in order
 * to write text in the given text size.
 * The default text size is 2.
 */
void setColumn(uint8_t column, uint8_t textSize = 2);

// -------------------------------printPermanent------------------------------------------------------------------------
/*====================================================================================*
 *                                printPermanent                                      *
 *====================================================================================*
 * This function prints on the screen permanent information which are  date,  time,
 * temperature, if the oven is running or a program is delayed,
 * and if so, the time left.
 */
void printPermanent(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second,
                    bool isRunning, uint16_t timeLeft, bool isDelayed, uint16_t timeBeforeStart);

/*====================================================================================*
 *                             clearPrintPermanent                                    *
 *====================================================================================*
 * This function clears the bottom of the screen where are printed the permanent
 * information.
 */
void clearPrintPermanent(void);

// --------------------------------printMainMenu------------------------------------------------------------------------
/*====================================================================================*
 *                                 printMainMenu                                      *
 *====================================================================================*
 * This function prints on the screen the Main Menu to select the next menu between
 * starting menu, showing information menu, setting time menu and curve managing menu.
 * If the oven has a delayed start, the starting menu turns into a menu to change the
 * value of the delay.
 * If the oven is running, the starting menu turns into a menu to stop the oven.
 */
void printMainMenu(uint16_t encoderPos, bool isRunning, bool isDelayed);

// ----------------------------printSelectCurveMenu------------------------------------------------------------------------
/*====================================================================================*
 *                             printSelectCurveMenu                                   *
 *====================================================================================*
 * This function prints on the screen the menu to select the curve between those on
 * the SD card.
 */
void printSelectCurveMenu(uint16_t encoderPos, uint8_t nbCurves, String arrayDisplayNames[], uint8_t numPage);

// -----------------------------printShowValuesMenu---------------------------------------------------------------------
/*====================================================================================*
 *                                  setCursorTab                                      *
 *====================================================================================*
 * This function set the cursor in order to print a table of time and temperature
 * of 22 columns maximum.
 */
void setCursorTab(uint8_t numTime, bool TimeOrTemp);

/*====================================================================================*
 *                                printTabTimeTemp                                    *
 *====================================================================================*
 * This function prints the values of a table of time and temperature
 * of 22 columns maximum.
 */
void printTabTimeTemp(TimeTemp arrayTimeTemp[], uint8_t len);

/*====================================================================================*
 *                                     drawTab                                        *
 *====================================================================================*
 * This function prints the ligns of a table of time and temperature of 22 columns.
 */
void drawTab(void);

/*====================================================================================*
 *                              printShowValuesMenu                                   *
 *====================================================================================*
 * This function prints the menu showing the table of values of the selected curve.
 */
void printShowValuesMenu(uint16_t encoderPos);

/*====================================================================================*
 *                                   printValues                                      *
 *====================================================================================*
 * This function prints the table of values of the selected curve.
 */
void printValues(String nameOfCurve);

// ------------------------------printPlotGraphMenu---------------------------------------------------------------------
/*====================================================================================*
 *                                   printCurve                                       *
 *====================================================================================*
 * This function prints the curve of temperature in function of time.
 */
void printCurve(TimeTemp arrayTimeTemp[], uint8_t len);

/*====================================================================================*
 *                               printPlotGraphMenu                                   *
 *====================================================================================*
 * This function prints the menu plotting the graph of the selected curve.
 */
void printPlotGraphMenu(uint16_t encoderPos);

/*====================================================================================*
 *                                   printGraph                                       *
 *====================================================================================*
 * This function prints the graph of the selected curve.
 */
void printGraph(String nameOfCurve);

// ----------------------------printStartOrDelayMenu--------------------------------------------------------------------
/*====================================================================================*
 *                             printStartOrDelayMenu                                  *
 *====================================================================================*
 * This function prints the menu to select whether we start directly the program or
 * we delay it.
 */
void printStartOrDelayMenu(uint16_t encoderPos, uint16_t tmax, uint8_t hour, uint8_t minute);

// --------------------------------printDelayMenu-----------------------------------------------------------------------
/*====================================================================================*
 *                                 printDelayMenu                                     *
 *====================================================================================*
 * This function prints the menu to select the value of the delay of the program.
 */
void printDelayMenu(uint16_t launchDelay, uint16_t tmax, uint8_t hour, uint8_t minute);

// --------------------------------printStopMenu------------------------------------------------------------------------
/*====================================================================================*
 *                                 printStopMenu                                      *
 *====================================================================================*
 * This function prints the menu to select whether we stop the running program.
 */
void printStopMenu(uint16_t encoderPos);

// ------------------------------printChangeDelayMenu-----------------------------------------------------------------------------------------------------------------
/*====================================================================================*
 *                               printChangeDelayMenu                                 *
 *====================================================================================*
 * This function prints the menu to select the new value of the delay of the program.
 */
void printChangeDelayMenu(uint16_t launchDelay, uint16_t tmax, uint8_t hour, uint8_t minute);

// --------------------------------printInfoMenu------------------------------------------------------------------------
/*====================================================================================*
 *                                 printInfoMenu                                      *
 *====================================================================================*
 * This function prints the temperature, the command of temperature, the state of the
 * relay and the time left before the end of the running program.
 */
void printInfoMenu(double temp, double command, bool isRunning, uint16_t timeLeft, bool increaseTemp);

// ------------------------------printSetTimeMenus--------------------------------------------------------------------------------------------------------------------
/*====================================================================================*
 *                               printSetTimeMenu                                     *
 *====================================================================================*
 * This function prints the menu to select whether to change the date or the time.
 */
void printSetTimeMenu(uint16_t encoderPos, uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute);

/*====================================================================================*
 *                              printSetTimeSubMenu                                   *
 *====================================================================================*
 * This function prints the menu to change the date or the time.
 */
void printSetTimeSubMenu(uint16_t Mode, uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute);

// ----------------------------printManageCurvesMenu--------------------------------------------------------------------
/*====================================================================================*
 *                             printManageCurvesMenu                                  *
 *====================================================================================*
 * This function prints the menu where we select what to do concerning the curves
 * stocked on the SD card: show onformation on them, create a new curve or delete one.
 */
void printManageCurvesMenu(uint16_t encoderPos);

/*====================================================================================*
 *                            printMaxNbCurvesErrorMenu                               *
 *====================================================================================*
 * This function prints the menu when we have reached the maximal number of curves
 * on the SD card, and we want to create a new one.
 */
void printMaxNbCurvesErrorMenu(void);
// -----------------------------printChooseNameMenu---------------------------------------------------------------------
/*====================================================================================*
 *                              printChooseNameMenu                                   *
 *====================================================================================*
 * This function prints the menu where we select the name of the new curve we want
 * to create.
 */
void printChooseNameMenu(uint16_t encoderPos, String displayName, uint8_t MAJminOth);

/*====================================================================================*
 *                              printNameContinueMenu                                 *
 *====================================================================================*
 * The name of the curve is already given.
 * This function prints the menu where we choose between continue with this name
 * or return back to take another name.
 */
void printNameContinueMenu(uint16_t encoderPos, String displayName);

// ------------------------------printSelectValuesMenu------------------------------------------------------------------
/*====================================================================================*
 *                            clearPrintSelectValuesMenu                              *
 *====================================================================================*
 * This function clears the top of the screen when we select values of time or
 * temperature.
 */
void clearPrintSelectValuesMenu(void);

/*====================================================================================*
 *                               printSelectValuesMenu                                *
 *====================================================================================*
 * This function prints the menu where we select the values of the new curve, to
 * select whether we create a new entry, delete one, validate or cancel.
 */
void printSelectValuesMenu(uint16_t encoderPos, TimeTemp arrayTimeTemp[], uint8_t len);

/*====================================================================================*
 *                                   printValues                                      *
 *====================================================================================*
 * This function prints the table of values of the curve we are creating.
 */
void printValues(TimeTemp arrayTimeTemp[], uint8_t len);
/*====================================================================================*
 *                              printSelectTimeTempMenu                               *
 *====================================================================================*
 * This function prints the menu where we select the values of the new curve, to
 * show the value we are modifying (time or temperature).
 */
void printSelectTimeTempMenu(TimeTemp arrayTimeTemp[], uint8_t len, uint16_t Mode);

// -------------------------------printPlotGraphMenu--------------------------------------------------------------------
/*====================================================================================*
 *                                printPlotGraphMenu                                  *
 *====================================================================================*
 * This function prints the menu where we show the graph we just have created.
 */
void printPlotGraphMenu(uint16_t encoderPos, TimeTemp arrayTimeTemp[], uint8_t len);

// -------------------------------printCreationMenu------------------------------------------------------------------------
/*====================================================================================*
 *                                printCreationMenu                                   *
 *====================================================================================*
 * This function prints the menu where we wait for the files to be writen.
 */
void printCreationMenu(void);

// -----------------------------printDeleteCurveMenu--------------------------------------------------------------------
/*====================================================================================*
 *                              printDeleteCurveMenu                                  *
 *====================================================================================*
 * This function prints the menu where we confirm we want to delete the selected curve.
 */
void printDeleteCurveMenu(uint16_t encoderPos, String nameOfCurve);

/*====================================================================================*
 *                               printNameErrorMenu                                   *
 *====================================================================================*
 * This function prints the menu when the chosen name of file to delete or create
 * is the one which is running.
 */
void printNameErrorMenu(String nameOfCurve, uint16_t Mode);

// -------------------------------printEndCycleMenu---------------------------------------------------------------------
/*====================================================================================*
 *                                printEndCycleMenu                                   *
 *====================================================================================*
 * This function prints the menu when the cycle has finished, when the buzzer is
 * running.
 */
void printEndCycleMenu(void);

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
