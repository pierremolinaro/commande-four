// ----------Always begin by this pragma----------
#pragma once

// ----------Libraries----------
#include <Wire.h>
#include <RtcDS1307.h>

// ----------Headers----------
#include "Defines.h"

// ----------Functions declaration----------
/*====================================================================================*
 *                                   initClock                                        *
 *====================================================================================*
 * This function initializes the communication with the clock.
 */
void initRealTimeClock (void) ;

/*====================================================================================*
 *                                    setTime                                         *
 *====================================================================================*
 * This function sets the time of the component.
 */
void setTime(uint16_t year, uint8_t month, uint8_t dayOfMonth, uint8_t hour, uint8_t minute, uint8_t second);

/*====================================================================================*
 *                                  updateTime                                        *
 *====================================================================================*
 * This function stocks the data read by the clock in a static variable now.
 */
void updateTime (void);

RtcDateTime currentDateTime (void) ;

/*====================================================================================*
 *                              getMinutesSince2000                                   *
 *====================================================================================*
 * This function returns the number of minutes since the 01/01/2000.
 */
uint32_t getMinutesSince2000(void);

/*====================================================================================*
 *                                  current****                                       *
 *====================================================================================*
 * These functions are used to get the current value of time.
 */
uint16_t currentYear(void);
uint8_t currentMonth(void);
uint8_t currentDay(void);
uint8_t currentHour(void);
uint8_t currentMinute(void);
uint8_t currentSecond(void);

/*====================================================================================*
 *                                    set****                                         *
 *====================================================================================*
 * These functions are used to set one component of time.
 */
void setYear(uint16_t year);
void setMonth(uint8_t month);
void setDay(uint8_t day);
void setHour(uint8_t hour);
void setMinute(uint8_t minute);
void resetSecond(void); // For the seconds we reset them to 0
