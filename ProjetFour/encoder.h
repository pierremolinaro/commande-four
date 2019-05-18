// ----------Always begin by this pragma----------
#pragma once

// ----------Libraries----------
#include <Arduino.h>

// ----------Headers----------
#include "Defines.h"

// ----------Functions declaration----------
/*====================================================================================*
 *                                 initEncoder                                        *
 *====================================================================================*
 * This function sets the two pins of the rotary encoder as inputs, and creates an 
 * attached interrupt activated every 1000µs.
 */
void initEncoder(void);

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   DEFINIR PLAGE ENCODEUR
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void definirPlageEncodeur (const uint32_t inMinValue, const uint32_t inCurrentValue, const uint32_t inMaxValue) ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   OBTENIR ENCODEUR
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

uint32_t getEncoderValue (void) ;

/*====================================================================================*
 *                               useRotaryEncoder                                     *
 *====================================================================================*
 * This function returns the position of the encoder,
 * this value varies between minValue and maxValue.
 */
uint16_t useRotaryEncoder(uint16_t minValue, uint16_t maxValue);

/*====================================================================================*
 *                               encoderPosition                                      *
 *====================================================================================*
 * This function is used to know in which subMenus we are going.
 * It returns the position of the encoder,
 * this value varies between 0 and nbMenus-1.
 */
uint16_t encoderPosition(uint16_t nbMenus);

/*====================================================================================*
 *                             setEncoderPosition                                     *
 *====================================================================================*
 * This function is used to set the encoder position to a definite value.
 * If we give no argument, if reset the position to zero.
 */
void setEncoderPosition(int16_t newPosition);
