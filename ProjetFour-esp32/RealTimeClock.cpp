// ----------Include the header----------
#include "RealTimeClock.h"
#include "gcc-diagnostics.h"

// ----------Static variables in the file----------
static RtcDS1307<TwoWire> Rtc(Wire);
static RtcDateTime gNow ;

// ----------Functions----------
/*====================================================================================*
 *                                   initClock                                        *
 *====================================================================================*
 * This function initializes the communication with the clock.
 */
void initRealTimeClock (void) {
    Rtc.Begin () ;
    Rtc.SetIsRunning (true) ;
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
void updateTime (void) {
  gNow = Rtc.GetDateTime () ;
}

RtcDateTime currentDateTime (void) {
  return Rtc.GetDateTime () ;
}

/*====================================================================================*
 *                              getMinutesSince2000                                   *
 *====================================================================================*
 * This function returns the number of minutes since the 01/01/2000.
 */
uint32_t getMinutesSince2000(void) {
    return (gNow.TotalSeconds() / 60) ;
}

/*====================================================================================*
 *                                  current****                                       *
 *====================================================================================*
 * These functions are used to get the current value of time.
 */
uint16_t currentYear(void) {
    return(gNow.Year());
}

uint8_t currentMonth(void) {
    return(gNow.Month());
}

uint8_t currentDay(void) {
    return(gNow.Day());
}

uint8_t currentHour(void) {
    return(gNow.Hour());
}

uint8_t currentMinute(void) {
    return(gNow.Minute());
}
uint8_t currentSecond(void) {
    return(gNow.Second());
}

/*====================================================================================*
 *                                    set****                                         *
 *====================================================================================*
 * These functions are used to set one component of time.
 */
void setYear(uint16_t year) {
    updateTime();
    setTime(year, gNow.Month(), gNow.Day(), gNow.Hour(), gNow.Minute(), gNow.Second());
}

void setMonth(uint8_t month) {
    updateTime();
    setTime(gNow.Year(), month, gNow.Day(), gNow.Hour(), gNow.Minute(), gNow.Second());
}

void setDay(uint8_t day) {
    updateTime();
    setTime(gNow.Year(), gNow.Month(), day, gNow.Hour(), gNow.Minute(), gNow.Second());
}

void setHour(uint8_t hour) {
    updateTime();
    setTime(gNow.Year(), gNow.Month(), gNow.Day(), hour, gNow.Minute(), gNow.Second());
}

void setMinute(uint8_t minute) {
    updateTime();
    setTime(gNow.Year(), gNow.Month(), gNow.Day(), gNow.Hour(), minute, gNow.Second());
}

// For the seconds we reset them to 0
void resetSecond(void) {
    updateTime();
    setTime(gNow.Year(), gNow.Month(), gNow.Day(), gNow.Hour(), gNow.Minute(), 0);
}
