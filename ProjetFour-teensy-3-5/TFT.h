#pragma once

//----------------------------------------------------------------------------------------------------------------------
//  Check board type
//----------------------------------------------------------------------------------------------------------------------

#include "check-board.h"

//----------------------------------------------------------------------------------------------------------------------

// ----------Libraries----------
#include <SPI.h>
#include <ILI9341_t3n.h>

// ----------Headers----------
#include "Defines.h"
#include "SDCard.h"

extern ILI9341_t3n tft ;

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
 *                                 clearScreen                                        *
 *====================================================================================*
 * This function clears all the screen except the bottom where are printed the
 * permanent information.
 */
void clearScreen(void);

/*====================================================================================*
 *                                   setLineColumnForTextSize                         *
 *====================================================================================*
 * This function sets the cursor at the given lign and at the first column in order
 * to write text in the given text size.
 * The default text size is 2.
 */
void setLineColumnForTextSize (const uint8_t inLine,
                               const uint8_t inColumn,
                               const uint8_t inTextSize,
                               const bool inAddHalfLine = false) ;

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

void printWithPadding (const String & inString, const uint32_t inSize) ;

void afficherCompteurErreurs (const char * inMessage, const uint32_t inNombreErreurs) ;

//----------------------------------------------------------------------------------------------------------------------

void printColoredStatus (const uint32_t inValue) ;
