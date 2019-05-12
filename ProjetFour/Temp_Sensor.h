// ----------Always begin by this pragma----------
#pragma once

// ----------Libraries----------
#include <Adafruit_MAX31855.h>

// ----------Headers----------
#include "Defines.h"

// ----------Functions declaration----------
/*====================================================================================*
 *                                  updateTemp                                        *
 *====================================================================================*
 * This function stocks the data read by the thermocouple in a static variable seqBin.
 */
void updateTemp(void);

/*====================================================================================*
 *                                 testErrorTemp                                      *
 *====================================================================================*
 * This function returns true if there is an error in the data read.
 */
bool testErrorTemp(void);

/*====================================================================================*
 *                                    getTemp                                         *
 *====================================================================================*
 * This function returns the temperature read with the thermocouple.
 */
double getTemp(void);
