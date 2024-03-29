#pragma once

//----------------------------------------------------------------------------------------------------------------------
//  Check board type
//----------------------------------------------------------------------------------------------------------------------

#include "check-board.h"

//----------------------------------------------------------------------------------------------------------------------

#include <Arduino.h>
#include "Defines.h"

//----------------------------------------------------------------------------------------------------------------------
//   INIT
//----------------------------------------------------------------------------------------------------------------------

void initEncoder (void);

//----------------------------------------------------------------------------------------------------------------------
//   ENCODER CLICK PRESSED
//----------------------------------------------------------------------------------------------------------------------

bool encoderClickPressed (void) ;

//----------------------------------------------------------------------------------------------------------------------
//   SET ENCODER RANGE
//----------------------------------------------------------------------------------------------------------------------

void setEncoderRange (const uint32_t inMinValue,
                      const uint32_t inCurrentValue,
                      const uint32_t inMaxValue,
                      const bool inRollover) ;

//----------------------------------------------------------------------------------------------------------------------
//   ENCODER VALUE DID CHANGE
//----------------------------------------------------------------------------------------------------------------------

bool encoderValueDidChange (void) ;

//----------------------------------------------------------------------------------------------------------------------
//   GET ENCODER VALUE
//----------------------------------------------------------------------------------------------------------------------

uint32_t getEncoderValue (void) ;

//----------------------------------------------------------------------------------------------------------------------
//   GET ENCODER MAX VALUE
//----------------------------------------------------------------------------------------------------------------------

uint32_t getEncoderMaxValue (void) ;

//----------------------------------------------------------------------------------------------------------------------

void runEncoderFromISR (void) ;

//----------------------------------------------------------------------------------------------------------------------
