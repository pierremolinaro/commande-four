#pragma once

//----------------------------------------------------------------------------------------------------------------------
//  Check board type
//----------------------------------------------------------------------------------------------------------------------

#include "check-board.h"

//----------------------------------------------------------------------------------------------------------------------
//   HEADERS
//----------------------------------------------------------------------------------------------------------------------

#include <RtcDS1307.h>

//----------------------------------------------------------------------------------------------------------------------
//   INIT OVEN CONTROL
//----------------------------------------------------------------------------------------------------------------------

void initOvenControl (void) ;

//----------------------------------------------------------------------------------------------------------------------
//   RUN OVEN CONTROL ONCE
//----------------------------------------------------------------------------------------------------------------------

void runOvenControlFromISR (void) ;

//----------------------------------------------------------------------------------------------------------------------
//   START OVEN IN AUTOMATIC MODE
//----------------------------------------------------------------------------------------------------------------------

void startOvenInAutomaticMode (const uint32_t inDelayInMinutes) ;

uint32_t delayForStartingInSeconds (void) ;

//----------------------------------------------------------------------------------------------------------------------
//   START OVEN IN MANUAL MODE
//----------------------------------------------------------------------------------------------------------------------

void startOvenInManualMode (const uint32_t inTemperatureReference) ; // Consigne en Celcius

//----------------------------------------------------------------------------------------------------------------------
//   SET TEMPERATURE REFERENCE IN MANUAL MODE
//----------------------------------------------------------------------------------------------------------------------

void setTemperatureReferenceInManualMode (const int32_t inTemperatureReference) ; // Consigne en Celcius

//----------------------------------------------------------------------------------------------------------------------
//   STOP OVEN
//----------------------------------------------------------------------------------------------------------------------

void stopOven (void) ;

//----------------------------------------------------------------------------------------------------------------------
//   OVEN IS RUNNING
//----------------------------------------------------------------------------------------------------------------------

bool ovenIsRunning (void) ;

//----------------------------------------------------------------------------------------------------------------------
//   RUNNING TIME (in seconds)
//----------------------------------------------------------------------------------------------------------------------

uint32_t ovenRunningTime (void) ;

//----------------------------------------------------------------------------------------------------------------------
//   TEMPERATURE REFERENCE
//----------------------------------------------------------------------------------------------------------------------

double temperatureReference (void) ;

//----------------------------------------------------------------------------------------------------------------------
