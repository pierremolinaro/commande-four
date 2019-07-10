#pragma once

//----------------------------------------------------------------------------------------------------------------------
//  Check board type
//----------------------------------------------------------------------------------------------------------------------

#include "check-board.h"

//----------------------------------------------------------------------------------------------------------------------

#include "Defines.h"

//----------------------------------------------------------------------------------------------------------------------
//   PROGRAM FILE NAMES MANAGMENT
//----------------------------------------------------------------------------------------------------------------------

 void resetProgramList (void) ;

void buildProgramFileNameArray (void) ;

uint32_t programFileCount (void) ;

String programFileNameAtIndex (const uint32_t inIndex) ;

//----------------------------------------------------------------------------------------------------------------------
//   PROGRAM FILE
//----------------------------------------------------------------------------------------------------------------------

bool readProgramFile (const String & inFileName) ;

String programName (void) ;

uint32_t programDurationInMinutes (void) ;

double programTemperatureReferenceForRunningTime (const uint32_t inRunningTimeInSeconds) ;

void plotGraph (void) ;

void printTable (const char * inTitle) ;

//----------------------------------------------------------------------------------------------------------------------
