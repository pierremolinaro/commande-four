// ----------Include the header----------
#include "Clock.h"

// ----------Static variables in the file----------
static RtcDS1307<TwoWire> Rtc(Wire);
static RtcDateTime now;

// ----------Functions----------
/*====================================================================================*
 *                                   initClock                                        *
 *====================================================================================*
 * This function initializes the communication with the clock.
 */
void initClock(void){
    Rtc.Begin();
    Rtc.SetIsRunning(true);
}

/*====================================================================================*
 *                                    setTime                                         *
 *====================================================================================*
 * This function sets the time of the component.
 */
void setTime(uint16_t year, uint8_t month, uint8_t dayOfMonth, uint8_t hour, uint8_t minute, uint8_t second) {
    RtcDateTime DateTime = RtcDateTime(year, month, dayOfMonth, hour, minute, second);
    Rtc.SetDateTime(DateTime);
}

/*====================================================================================*
 *                                  updateTime                                        *
 *====================================================================================*
 * This function stocks the data read by the clock in a static variable now.
 */
void updateTime(void) {
    now = Rtc.GetDateTime();
}

/*====================================================================================*
 *                              getMinutesSince2000                                   *
 *====================================================================================*
 * This function returns the number of minutes since the 01/01/2000.
 */
uint32_t getMinutesSince2000(void) {
    return(now.TotalSeconds()/60);
}

/*====================================================================================*
 *                                  current****                                       *
 *====================================================================================*
 * These functions are used to get the current value of time.
 */
uint16_t currentYear(void) {
    return(now.Year());
}

uint8_t currentMonth(void) {
    return(now.Month());
}

uint8_t currentDay(void) {
    return(now.Day());
}

uint8_t currentHour(void) {
    return(now.Hour());
}

uint8_t currentMinute(void) {
    return(now.Minute());
}
uint8_t currentSecond(void) {
    return(now.Second());
}

/*====================================================================================*
 *                                    set****                                         *
 *====================================================================================*
 * These functions are used to set one component of time.
 */
void setYear(uint16_t year) {
    updateTime();
    setTime(year, now.Month(), now.Day(), now.Hour(), now.Minute(), now.Second());
}

void setMonth(uint8_t month) {
    updateTime();
    setTime(now.Year(), month, now.Day(), now.Hour(), now.Minute(), now.Second());
}

void setDay(uint8_t day) {
    updateTime();
    setTime(now.Year(), now.Month(), day, now.Hour(), now.Minute(), now.Second());
}

void setHour(uint8_t hour) {
    updateTime();
    setTime(now.Year(), now.Month(), now.Day(), hour, now.Minute(), now.Second());
}

void setMinute(uint8_t minute) {
    updateTime();
    setTime(now.Year(), now.Month(), now.Day(), now.Hour(), minute, now.Second());
}

// For the seconds we reset them to 0
void resetSecond(void) {
    updateTime();
    setTime(now.Year(), now.Month(), now.Day(), now.Hour(), now.Minute(), 0);
}
