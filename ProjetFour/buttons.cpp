// ----------Include the header----------
#include "buttons.h"

// ----------Static variables in the file----------
static bool lastClickState = true;
static bool clickPress = false;
static hw_timer_t * timer = NULL;

// ----------Functions----------
/*====================================================================================*
 *                                  initButtons                                       *
 *====================================================================================*
 * This function sets the pins of the buttons as inputs, and creates an 
 * attached interrupt activated every 100µs.
 */
void initButtons(void) {
    pinMode(clickPin, INPUT_PULLUP); // setup the button pin
    timer = timerBegin(1, 1000, true);
    timerAttachInterrupt(timer, &clickISR, true);
    timerAlarmWrite(timer, 100, true); // 100 µs
    timerAlarmEnable(timer);
}

/*====================================================================================*
 *                                    clickISR                                        *
 *====================================================================================*
 * This function updates the states of the button, keeping the last state.
 * It is an interruption function stocked in the RAM.
 */
void IRAM_ATTR clickISR(void) {
    if (digitalRead(clickPin) == true && lastClickState == false) {
        clickPress = true;
    }
    lastClickState = digitalRead(clickPin);
}

/*====================================================================================*
 *                                 clickPressed                                       *
 *====================================================================================*
 * This function returns true on a rising edge of the click, else returns false.
 */
bool clickPressed(void) {
    if (clickPress) {
        clickPress = false;
        return(true);
    } else {
        return(false);
    }
}
