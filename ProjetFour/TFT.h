#pragma once

//----------------------------------------------------------------------------------------------------------------------
//  Check board type
//----------------------------------------------------------------------------------------------------------------------

#ifndef ARDUINO_MH_ET_LIVE_ESP32MINIKIT
  #error "Select 'MH ET LIVE ESP32MiniKit' board"
#endif

//----------------------------------------------------------------------------------------------------------------------

// ----------Libraries----------
#include <SPI.h>
#include <TFT_eSPI.h>

// ----------Headers----------
#include "Defines.h"
#include "SDCard.h"

extern TFT_eSPI tft ;

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
 *                                   setLineForTextSize                                          *
 *====================================================================================*
 * This function sets the cursor at the given lign and at the first column in order
 * to write text in the given text size.
 * The default text size is 2.
 */
void setLineForTextSize (const uint8_t inLine, const uint8_t inTextSize, const bool inAddHalfLine = false) ;

/*====================================================================================*
 *                                  setColumn                                         *
 *====================================================================================*
 * This function sets the cursor at the same lign and at the given column in order
 * to write text in the given text size.
 * The default text size is 2.
 */
void setColumnForTextSize (const uint8_t inColumn, const uint8_t inTextSize) ;

void setMenuColor (const bool inIsSelected, const bool inIsEditing) ;

void printWithPadding (const char * inString, const uint32_t inSize) ;

void afficherCompteurErreurs (const char * inMessage, const uint32_t inNombreErreurs) ;

//----------------------------------------------------------------------------------------------------------------------

void printColoredStatus (const uint32_t inValue) ;
