// ----------Always begin by this pragma----------
#pragma once

//----------------------------------------------------------------------------------------------------------------------
//  Check board type
//----------------------------------------------------------------------------------------------------------------------

#include "check-board.h"

//----------------------------------------------------------------------------------------------------------------------

// ----------Libraries----------
#include <SdFat.h>
#include <SPI.h>

// ----------Headers----------
#include "Defines.h"

extern SdFat SD ;

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
