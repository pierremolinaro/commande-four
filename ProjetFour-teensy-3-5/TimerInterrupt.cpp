#include <TimerOne.h>

#include "TimerInterrupt.h"
#include "RotaryEncoder.h"
#include "TemperatureSensor.h"
#include "OvenControl.h"

//----------------------------------------------------------------------------------------------------------------------

static uint32_t gSensorDelayCounter = 0 ;
static uint32_t gOvenDelayCounter = 1062 ;

//----------------------------------------------------------------------------------------------------------------------

static void interruptServiceRoutine (void) {
  runEncoderFromISR () ;
  if (gOvenDelayCounter == 0) {
    gOvenDelayCounter = 1000 - 1 ; // Execution every 1000 ms
    runOvenControlFromISR () ;
  }else{
    gOvenDelayCounter -= 1 ;
  }
  if (gSensorDelayCounter == 0) {
    gSensorDelayCounter = 125 - 1 ; // Execution every 125 ms
    runTemperatureSensorFromISR () ;
  }else{
    gSensorDelayCounter -= 1 ;
  }
}

//----------------------------------------------------------------------------------------------------------------------

void configureTimerInterrupt (void) {
  Timer1.initialize (1000) ; // 1000 µs
  Timer1.attachInterrupt (interruptServiceRoutine);
}

//----------------------------------------------------------------------------------------------------------------------
