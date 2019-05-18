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
 *                                 clickPressed                                       *
 *====================================================================================*
 * This function returns true on a rising edge of the click, else returns false.
 */
bool clickPressed(void);
