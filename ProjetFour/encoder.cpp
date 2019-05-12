// ----------Include the header----------
#include "encoder.h"

// ----------Static variables in the file----------
static int16_t encoderPos = 0; // This variable stores our current value of encoder position.
static hw_timer_t * timer = NULL;

// ----------Functions----------
/*====================================================================================*
 *                                 initEncoder                                        *
 *====================================================================================*
 * This function sets the two pins of the rotary encoder as inputs, and creates an 
 * attached interrupt activated every 1000µs.
 */
void initEncoder(void) {
    pinMode(pinA, INPUT_PULLUP); // set pinA as an input
    pinMode(pinB, INPUT_PULLUP); // set pinB as an input
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &encoderISR, true);
    timerAlarmWrite(timer, 1000, true); // 1000 µs
    timerAlarmEnable(timer);
}

/*====================================================================================*
 *                                  encoderISR                                        *
 *====================================================================================*
 * This function is used to change the value of the encoder when we turn it.
 * It is an interruption function stocked in the RAM.
 */
void IRAM_ATTR encoderISR(void) {
    static bool pinALast = HIGH;
    bool pinACurrent = digitalRead(pinA);
    if ((pinALast == LOW) && (pinACurrent == HIGH)) { //Rising edge
        // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
        if (digitalRead(pinB) == LOW) {
            encoderPos ++;
        }
        else {
            encoderPos --;
        }
    }
    pinALast = pinACurrent;
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
uint16_t encoderPosition(uint16_t nbMenus) {
    return(useRotaryEncoder(0, nbMenus-1));
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
