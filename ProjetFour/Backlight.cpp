#include "Backlight.h"
#include "Defines.h"

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   BACKLIGHT
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static const uint32_t PWM_MAX_INTENSITY = 1 << BACKLIGHT_RESOLUTION ;

static uint32_t gExtinctionDate = 60 * 1000 ; // Initialement, date extinction après le démarrage (en ms)
static uint32_t gIntensity = PWM_MAX_INTENSITY ; // PWM_MAX_INTENSITY --> eclairage max, 0 -> éteint

//······················································································································

bool backlightIsON (void) {
  return gIntensity != 0 ;
}

//······················································································································

void initBacklight (void) {
  pinMode (BUTTON_ACTIVATE_BACKLIGHT, INPUT) ;
  ledcAttachPin (PIN_BACKLIGHT, PWM_CHANNEL_BACKLIGHT) ;
  ledcSetup (PWM_CHANNEL_BACKLIGHT, BACKLIGHT_FREQUENCY, BACKLIGHT_RESOLUTION);
//  Serial.print ("Backlight max: ") ; Serial.println (PWM_MAX_INTENSITY) ;
  updateBacklight () ;
}

//······················································································································

void extendBackLightDuration (void) {
  const uint32_t newExtinctionDate = millis () + 60 * 1000 ;
  if (gExtinctionDate < newExtinctionDate) {
    gExtinctionDate = newExtinctionDate ;
    gIntensity = PWM_MAX_INTENSITY ;
  }
}

//······················································································································

void updateBacklight (void) {
// Appuyer sur le bouton prolonge la durée de retro-éclairage
  if (!digitalRead (BUTTON_ACTIVATE_BACKLIGHT)) {
    extendBackLightDuration () ;
  }
//--- Contrôler le retro-éclairage
  if ((gExtinctionDate < millis ()) && (gIntensity > 0)) {
    gIntensity *= 9 ;
    gIntensity /= 10 ;
    gExtinctionDate += 100 ; // Période de diminution de l'intensité
//    Serial.print ("Extinction: ") ;
//    Serial.print (gIntensity) ;
//    Serial.print (", date: ") ;
//    Serial.println (gExtinctionDate) ;
  }
  ledcWrite (PWM_CHANNEL_BACKLIGHT, PWM_MAX_INTENSITY - gIntensity) ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
