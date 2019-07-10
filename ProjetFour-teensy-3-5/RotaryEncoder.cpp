//----------------------------------------------------------------------------------------------------------------------
//   HEADER INCLUDES
//----------------------------------------------------------------------------------------------------------------------

#include "RotaryEncoder.h"
#include "Backlight.h"
#include "gcc-diagnostics.h"

//----------------------------------------------------------------------------------------------------------------------
//   STATIC VARIABLES
//----------------------------------------------------------------------------------------------------------------------

static int16_t encoderPos = 0; // This variable stores our current value of encoder position.

//--- Rotary variables
static uint32_t gMinEncoderValue ;
static uint32_t gCurrentEncoderValue ;
static uint32_t gMaxEncoderValue ;
static uint32_t gPreviousEncoderValue ;
static bool gRollover ;

//--- Click variables
static bool gLastClickState = true ;
static volatile bool gClickPressed = false ;

//----------------------------------------------------------------------------------------------------------------------
//   ENCODER INTERRUPT SERVICE ROUTINE
//----------------------------------------------------------------------------------------------------------------------

void runEncoderFromISR (void) {
  static bool pinALast = HIGH ;
  const bool pinACurrent = digitalRead (PIN_ENCODER_A);
  if ((pinALast == LOW) && (pinACurrent == HIGH)) { //Rising edge
        // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
    if (digitalRead (PIN_ENCODER_B) == LOW) {
       encoderPos ++ ;
       if (gCurrentEncoderValue <  gMaxEncoderValue) {
         gCurrentEncoderValue += 1 ;
       }else if (gRollover) {
         gCurrentEncoderValue = gMinEncoderValue ;
       }
    }else{
      encoderPos --;
      if (gCurrentEncoderValue > gMinEncoderValue) {
        gCurrentEncoderValue -= 1 ;
       }else if (gRollover) {
         gCurrentEncoderValue = gMaxEncoderValue ;
      }
    }
  }
  pinALast = pinACurrent;
//--- Handle encoder push button
  const bool newClickState = digitalRead (PIN_ENCODER_CLICK) == HIGH ;
  if (gLastClickState && !newClickState) {
    gClickPressed = true ;
  }
  gLastClickState = newClickState ;

}

//----------------------------------------------------------------------------------------------------------------------
//   INIT
//----------------------------------------------------------------------------------------------------------------------

void initEncoder (void) {
  pinMode (PIN_ENCODER_CLICK, INPUT); // setup the button pin
  pinMode (PIN_ENCODER_A, INPUT) ; // set pinA as an input
  pinMode (PIN_ENCODER_B, INPUT) ; // set pinB as an input
}

//----------------------------------------------------------------------------------------------------------------------
//   SET ENCODER RANGE
//----------------------------------------------------------------------------------------------------------------------

void setEncoderRange (const uint32_t inMinValue,
                      const uint32_t inCurrentValue,
                      const uint32_t inMaxValue,
                      const bool inRollover) {
  gRollover = inRollover ;
  gMinEncoderValue = inMinValue ;
  gCurrentEncoderValue = inCurrentValue ;
  gMaxEncoderValue = inMaxValue ;
  if (gMaxEncoderValue < gMinEncoderValue) {
    gMaxEncoderValue = gMinEncoderValue ;
  }
  if (gCurrentEncoderValue > gMaxEncoderValue) {
    gCurrentEncoderValue = gMaxEncoderValue ;
  }else if (gCurrentEncoderValue < gMinEncoderValue) {
    gCurrentEncoderValue = gMinEncoderValue ;
  }
  gPreviousEncoderValue = gCurrentEncoderValue ;
}

//----------------------------------------------------------------------------------------------------------------------
//   ENCODER VALUE DID CHANGE
//----------------------------------------------------------------------------------------------------------------------

bool encoderValueDidChange (void) {
  return gPreviousEncoderValue != gCurrentEncoderValue ;
}

//----------------------------------------------------------------------------------------------------------------------
//   GET VALUE
//----------------------------------------------------------------------------------------------------------------------

uint32_t getEncoderValue (void) {
  if (gPreviousEncoderValue != gCurrentEncoderValue) {
    gPreviousEncoderValue = gCurrentEncoderValue ;
    extendBackLightDuration () ;
  }
  return gCurrentEncoderValue ;
}

//----------------------------------------------------------------------------------------------------------------------
//   GET ENCODER MAX VALUE
//----------------------------------------------------------------------------------------------------------------------

uint32_t getEncoderMaxValue (void) {
  return gMaxEncoderValue ;
}


//----------------------------------------------------------------------------------------------------------------------
//   ENCODER CLICK PRESSED
//----------------------------------------------------------------------------------------------------------------------

bool encoderClickPressed (void) {
  bool result = false ;
  if (gClickPressed) {
    if (backlightIsON ()) {
      result = true ;
    }
    gClickPressed = false ;
    extendBackLightDuration () ;
  }
  return result ;
}

//----------------------------------------------------------------------------------------------------------------------
