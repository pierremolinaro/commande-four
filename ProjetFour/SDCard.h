// ----------Always begin by this pragma----------
#pragma once

//----------------------------------------------------------------------------------------------------------------------
//  Check board type
//----------------------------------------------------------------------------------------------------------------------

#ifndef ARDUINO_MH_ET_LIVE_ESP32MINIKIT
  #error "Select 'MH ET LIVE ESP32MiniKit' board"
#endif

//----------------------------------------------------------------------------------------------------------------------

// ----------Libraries----------
#include <SD.h>
#include <SPI.h>

// ----------Headers----------
#include "Defines.h"

// ----------Functions declaration----------
/*====================================================================================*
 *                                  initSDcard                                        *
 *====================================================================================*
 * This function inits the SD connection defining the used SPI,
 * and prints information about the SD card.
 */
void initSDcard (void);

void updateSDCardStatus (void) ;

enum class SDCardStatus { noCard, insertedNotMounted, mounted } ;

SDCardStatus sdCardStatus (void) ;

uint32_t mountIndex (void) ; // Incremented each time a card is mounted

bool directoryExists (const String & inPath) ;

bool createDirectory (const String & inPath) ;

File openFileForAppending (const String & inFilePath) ;

File openFileForCreation (const String & inFilePath) ;

// --------------------------------BasicFunctions----------------------------------------------------------------------------------------------------------------------
/*====================================================================================*
 *                                   writeFile                                        *
 *====================================================================================*
 * This function creates a file (or replace it) and write on it the message given.
 */
void writeFile(String path, String message);

/*====================================================================================*
 *                                  appendFile                                        *
 *====================================================================================*
 * This function appends to the file the message given, or creates a file with the
 * message.
 */
void appendFile(String path, String message);

/*====================================================================================*
 *                                  renameFile                                        *
 *====================================================================================*
 * This function renames the first file given with the second name given.
 */
void renameFile(String path1, String path2);

/*====================================================================================*
 *                                  deleteFile                                        *
 *====================================================================================*
 * This function deletes the file.
 */
void deleteFile(String path);

// --------------------------------BasicFunctions----------------------------------------------------------------------------------------------------------------------
/*====================================================================================*
 *                                    giveTemp                                        *
 *====================================================================================*
 * This function gives the wanted temperature read on the SD card,
 * it gives the temperature corresponding to the time just after the given moment.
 */
float giveTemp(String nameOfCurve, uint16_t t);

/*====================================================================================*
 *                                  numberFiles                                       *
 *====================================================================================*
 * This function gives the number of files in the given directory.
 */
uint8_t numberFiles(String dirname);

/*====================================================================================*
 *                                getDisplayNames                                     *
 *====================================================================================*
 * This function gives the names of the files in the values directory.
 */
void getDisplayNames(String outFileNameArray[]);

/*====================================================================================*
 *                              extractCurveNameFile                                  *
 *====================================================================================*
 * This function gives the name of the curve, by giving the full name
 * in the curve directory.
 */
String extractCurveNameFile(String fullCurveName);

/*====================================================================================*
 *                              extractValuesNameFile                                 *
 *====================================================================================*
 * This function gives the name of the curve, by giving the full name
 * in the values directory.
 */
String extractValuesNameFile(String fullValuesName);

/*====================================================================================*
 *                                    timeMax                                         *
 *====================================================================================*
 * This function gives the maximal time of the given curve.
 */
uint16_t timeMax(String nameOfCurve);

/*====================================================================================*
 *                                    tempMax                                         *
 *====================================================================================*
 * This function gives the maximal temperature of the given array.
 */
uint16_t tempMax(TimeTemp arrayTimeTemp[], uint8_t len);

/*====================================================================================*
 *                                 createValues                                       *
 *====================================================================================*
 * This function creates the file in the values directory with the given name,
 * and writes in it the values of the given array.
 */
void createValues(String displayName, TimeTemp arrayTimeTemp[], uint8_t len);

/*====================================================================================*
 *                                 extractValues                                      *
 *====================================================================================*
 * This function extracts the values in the given file and writes them in
 * the given array and returns the size of the array.
 */
uint8_t extractValues(TimeTemp arrayTimeTemp[], String pathValues);

/*====================================================================================*
 *                                  createCurve                                       *
 *====================================================================================*
 * This function creates the file in the curve directory with the given name,
 * and writes in it the values of temperature and time every minute,
 * by creating the broken lign.
 */
void createCurve(String pathCurve, TimeTemp arrayTimeTemp[], uint8_t len);

/*====================================================================================*
 *                                  createAll                                         *
 *====================================================================================*
 * This function creates the two files in both the values and curve directories.
 */
void createAll(String displayName, TimeTemp arrayTimeTemp[], uint16_t len);

/*====================================================================================*
 *                                 deleteCurve                                        *
 *====================================================================================*
 * This function deletes the two files in both the values and curve directories.
 */
void deleteCurve(String nameOfCurve);

/*====================================================================================*
 *                                stringInArray                                       *
 *====================================================================================*
 * This function returns true is the string is in the array.
 */
bool stringInArray(String str, String strArray[], uint8_t len);
