#include "Backlight.h"
#include "Defines.h"

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//   GERER RETRO ECLAIRAGE
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static const uint32_t PWM_MAX_INTENSITE = 1 << RESOLUTION_RETRO_ECLAIRAGE ;

static uint32_t gDateExtinction = 60 * 1000 ; // Initialement, date extinction après le démarrage (en ms)
static uint32_t gIntensite = PWM_MAX_INTENSITE ; // PWM_MAX_INTENSITE --> eclairage max, 0 -> éteint

//······················································································································

uint32_t intensiteRetroEclairage (void) {
  return gIntensite ;
}

//······················································································································

void initialiserRetroEclairage (void) {
  pinMode (BOUTON_BTN, INPUT) ;
  ledcAttachPin (PIN_BACKLIGHT, CANAL_PWM_RETRO_ECLAIRAGE) ;
  ledcSetup (CANAL_PWM_RETRO_ECLAIRAGE, FREQUENCE_RETRO_ECLAIRAGE, RESOLUTION_RETRO_ECLAIRAGE);
//  Serial.print ("Backlight max: ") ; Serial.println (PWM_MAX_INTENSITE) ;
  gererRetroEclairage () ;
}

//······················································································································

void prolongerRetroEclairage (void) {
  const uint32_t nouvelleDateExtinction = millis () + 60 * 1000 ;
  if (gDateExtinction < nouvelleDateExtinction) {
    gDateExtinction = nouvelleDateExtinction ;
    gIntensite = PWM_MAX_INTENSITE ;
  }
}

//······················································································································

void gererRetroEclairage (void) {
// Appuyer sur le bouton prolonge la durée de retro-éclairage
  if (!digitalRead (BOUTON_BTN)) {
    prolongerRetroEclairage () ;
  }
//--- Contrôler le retro-éclairage
  if ((gDateExtinction < millis ()) && (gIntensite > 0)) {
    gIntensite *= 9 ;
    gIntensite /= 10 ;
    gDateExtinction += 100 ; // Période de diminution de l'intensité
//    Serial.print ("Extinction: ") ;
//    Serial.print (gIntensite) ;
//    Serial.print (", date: ") ;
//    Serial.println (gDateExtinction) ;
  }
  ledcWrite (CANAL_PWM_RETRO_ECLAIRAGE, PWM_MAX_INTENSITE - gIntensite) ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
