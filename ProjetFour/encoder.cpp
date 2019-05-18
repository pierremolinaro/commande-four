// ----------Include the header----------
#include "encoder.h"
#include "Backlight.h"

// ----------Static variables in the file----------
static int16_t encoderPos = 0; // This variable stores our current value of encoder position.
static hw_timer_t * timer = NULL;

static uint32_t gMinEncoderValue ;
static uint32_t gCurrentEncoderValue ;
static uint32_t gMaxEncoderValue ;

// ----------Functions----------

/*====================================================================================*
 *                                  encoderISR                                        *
 *====================================================================================*
 * This function is used to change the value of the encoder when we turn it.
 * It is an interruption function stocked in the RAM.
 */
static void IRAM_ATTR encoderISR (void) {
  static bool pinALast = HIGH ;
  const bool pinACurrent = digitalRead (PIN_ENCODEUR_A);
  if ((pinALast == LOW) && (pinACurrent == HIGH)) { //Rising edge
        // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
    if (digitalRead (PIN_ENCODEUR_B) == LOW) {
       encoderPos ++ ;
       if (gCurrentEncoderValue <  gMaxEncoderValue) {
         gCurrentEncoderValue += 1 ;
       }
    }else{
      encoderPos --;
      if (gCurrentEncoderValue > gMinEncoderValue) {
        gCurrentEncoderValue -= 1 ;
      }
    }
  }
  pinALast = pinACurrent;
}

/*====================================================================================*
 *                                 initEncoder                                        *
 *====================================================================================*
 * This function sets the two pins of the rotary encoder as inputs, and creates an 
 * attached interrupt activated every 1000µs.
 */
void initEncoder (void) {
    pinMode (PIN_ENCODEUR_A, INPUT) ; // set pinA as an input
    pinMode (PIN_ENCODEUR_B, INPUT) ; // set pinB as an input
    timer = timerBegin (NUMERO_TIMER_ENCODER_NUMERIQUE, 80, true);
    timerAttachInterrupt(timer, encoderISR, true);
    timerAlarmWrite (timer, 1000, true); // 1000 µs
    timerAlarmEnable (timer) ;
}

/*====================================================================================*
 *                               useRotaryEncoder                                     *
 *====================================================================================*
 * This function returns the position of the encoder,
 * this value varies between minValue and maxValue.
 */
uint16_t useRotaryEncoder(uint16_t minValue, uint16_t maxValue) {
  if (encoderPos > maxValue) setEncoderPosition(minValue);
  else if (encoderPos < minValue) setEncoderPosition(maxValue);
  return(encoderPos);
}

/*====================================================================================*
 *                               encoderPosition                                      *
 *====================================================================================*
 * This function is used to know in which subMenus we are going.
 * It returns the position of the encoder,
 * this value varies between 0 and nbMenus-1.
 */
static uint16_t gPreviousEncoderPosition = 0 ;

uint16_t encoderPosition (uint16_t nbMenus) {
  const uint16_t encoderPosition = useRotaryEncoder (0, nbMenus-1) ;
  if (gPreviousEncoderPosition != encoderPosition) {
    gPreviousEncoderPosition = encoderPosition ;
    prolongerRetroEclairage () ;
  }
  return encoderPosition ;
}

/*====================================================================================*
 *                             setEncoderPosition                                     *
 *====================================================================================*
 * This function is used to set the encoder position to a definite value.
 * If we give no argument, if reset the position to zero.
 */
void setEncoderPosition(int16_t newPosition) {
    encoderPos = newPosition;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   DEFINIR PLAGE ENCODEUR
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void definirPlageEncodeur (const uint32_t inMinValue, const uint32_t inCurrentValue, const uint32_t inMaxValue) {
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
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   OBTENIR ENCODEUR
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

uint32_t getEncoderValue (void) {
  return gCurrentEncoderValue ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
