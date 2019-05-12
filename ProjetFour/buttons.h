// ----------Always begin by this pragma----------
#pragma once

// ----------Libraries----------
#include <Arduino.h>

// ----------Headers----------
#include "Defines.h"

// ----------Functions declaration----------
/*====================================================================================*
 *                                  initButtons                                       *
 *====================================================================================*
 * This function sets the pins of the buttons as inputs, and creates an 
 * attached interrupt activated every 100µs.
 */
void initButtons(void);

/*====================================================================================*
 *                                    clickISR                                        *
 *====================================================================================*
 * This function updates the states of the button, keeping the last state.
 */
void IRAM_ATTR clickISR(void);

/*====================================================================================*
 *                                 clickPressed                                       *
 *====================================================================================*
 * This function returns true on a rising edge of the click, else returns false.
 */
bool clickPressed(void);
