// ----------Include the header----------
#include "buttons.h"
#include "Backlight.h"

// ----------Static variables in the file----------
static bool gLastClickState = true ;
static volatile bool gClickPressed = false ;
static hw_timer_t * gTimer = NULL ;

// ----------Functions----------
/*====================================================================================*
 *                                    clickISR                                        *
 *====================================================================================*
 * This function updates the states of the button, keeping the last state.
 * It is an interruption function stocked in the RAM.
 */
void IRAM_ATTR clickISR (void) {
  const bool clickOff = digitalRead (PIN_CLIC_ENCODEUR) ;
  if (gLastClickState && !clickOff) {
    gClickPressed = true ;
  }
  gLastClickState = clickOff ;
}

/*====================================================================================*
 *                                  initButtons                                       *
 *====================================================================================*
 * This function sets the pins of the buttons as inputs, and creates an 
 * attached interrupt activated every 100µs.
 */
void initButtons (void) {
  pinMode (PIN_CLIC_ENCODEUR, INPUT); // setup the button pin
  gTimer = timerBegin (NUMERO_TIMER_CLIC_ENCODEUR, 80, true) ;
  timerAttachInterrupt (gTimer, clickISR, true);
  timerAlarmWrite (gTimer, 50000, true) ; // 50 ms
  timerAlarmEnable (gTimer) ;
}

/*====================================================================================*
 *                                 clickPressed                                       *
 *====================================================================================*
 * This function returns true on a rising edge of the click, else returns false.
 */
bool clickPressed (void) {
  bool result = false ;
  if (gClickPressed) {
    if (intensiteRetroEclairage () > 0) {
      result = true ;
    }
    gClickPressed = false ;
    prolongerRetroEclairage () ;
  }
  return result ;
}
