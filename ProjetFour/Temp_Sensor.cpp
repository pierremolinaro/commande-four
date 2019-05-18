#include "Temp_Sensor.h"
#include "Defines.h"

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// La lecture par (soft) SPI renvoie une valeur sur 32 bits dont la composition est la suivante :
//      D31 ... D18 : 14 bits signés donnant la température en quart de Celcius
//      D17 : réservé, toujours lu à 0
//      D16 : 1 --> faute, 0 --> ok
//      D15 ... D4 : 12 bits donnant la température interne (inutilisés)
//      D3 : réservé, toujours lu à 0
//      D2 : 0 --> ok, 1 --> SCV Fault : « the thermocouple is short-circuited to VCC »
//      D1 : 0 --> ok, 1 --> SCG Fault : « the thermocouple is short-circuited to GND »
//      D0 : 0 --> ok, 1 -->  OC Fault : « the thermocouple is open (no connections) »
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//  Interruptions périodiques de l'ESP32 (https://techtutorialsx.com/2017/10/07/esp32-arduino-timer-interrupts/)
//    - fréquence de base 80 MHz
//    - prescaler qui divise cette fréquence par 2 à 65536
//    - Attention, le numéro du timer peut entrer en conflit avec les PWM

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//  Variables statiques
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static hw_timer_t * gTimer = NULL ;
static const uint32_t TAILLE_TABLEAU_MESURES = 8 ;
static volatile uint32_t gTableauMesures [TAILLE_TABLEAU_MESURES] ;
static uint32_t gIndiceTableauMesure ; // Entre 0 et TAILLE_TABLEAU_MESURES - 1

static volatile uint32_t gNombreMesuresBrutesIncoherentesRejetees ;
static volatile uint32_t gNombreMesuresBrutesIncorrectes ;
static uint32_t gNombreMesuresMoyennesInvalides ;

static uint32_t gErreur ;
static double gMesure ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static void IRAM_ATTR routineInterruptionTimer (void) {
//--- Faire la mesure
  uint32_t rawValue = 0 ;
  digitalWrite (MAX31855_CS, LOW) ;
  delayMicroseconds (1) ;
  for (int32_t i = 31 ; i >= 0 ; i--) {
    digitalWrite (MAX31855_CLK, LOW);
    delayMicroseconds (1) ;
    rawValue <<= 1 ;
    if (digitalRead (MAX31855_DO)) {
      rawValue |= 1 ;
    }
    digitalWrite (MAX31855_CLK, HIGH) ;
    delayMicroseconds (1) ;
  }
  digitalWrite (MAX31855_CS, HIGH) ;
//--- Accepter la mesure si D3 et D17 sont à zéro
  const uint32_t MASQUE_ACCEPTATION = (1 << 3) | (1 << 17) ;
  if ((rawValue & MASQUE_ACCEPTATION) != 0) {
    gNombreMesuresBrutesIncoherentesRejetees += 1 ;
    rawValue = UINT32_MAX ; // Indiquer une valeur incohérente
  }else if (erreurCapteurTemperature () != 0) { //--- Comptabiliser le nombre de mesures incorrectes
    gNombreMesuresBrutesIncorrectes += 1 ;
  }
//--- Entrer la mesure dans le tableau
  gTableauMesures [gIndiceTableauMesure] = rawValue ;
  gIndiceTableauMesure += 1 ;
  if (gIndiceTableauMesure == TAILLE_TABLEAU_MESURES) {
    gIndiceTableauMesure = 0 ;
  }
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//  Configurer le thermo-couple
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void initialiserThermoCouple (void) {
  pinMode (MAX31855_CS, OUTPUT) ;
  digitalWrite (MAX31855_CS, HIGH) ;
  pinMode (MAX31855_CLK, OUTPUT) ;
  digitalWrite (MAX31855_CLK, HIGH) ;
  pinMode (MAX31855_DO, INPUT) ;
//--- Configurer le timer
  gTimer = timerBegin (NUMERO_TIMER_MESURE_TEMPERATURE, 80, true) ; // Toutes les 1µs, incrémentation
  timerAttachInterrupt (gTimer, routineInterruptionTimer, true) ; // Attacher la routine d'interruption
  timerAlarmWrite (gTimer, 125000, true) ; // Interruption toutes les 125 000 µs = 125 ms
  timerAlarmEnable (gTimer) ; // Démarrer le timer
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//  updateTemp
// This function writes the data read from the thermocouple in a static variable gRawValue.
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void updateTemp (void) {
  int32_t nombreMesuresValides = 0 ;
  uint32_t indicateursErreur = 0 ; // 0 -> Pas d'erreur
  int32_t mesuresCumulees = 0 ;
  for (uint32_t i=0 ; i<TAILLE_TABLEAU_MESURES ; i++) {
    const uint32_t mesure = gTableauMesures [i] ;
    if (mesure != UINT32_MAX) { // Mesure incohérente rejetée
      const uint32_t erreur = mesure & 0x07 ;
      indicateursErreur |= erreur ;
      if (erreur == 0) {
        int32_t v = (int32_t) mesure ;
        v >>= 18 ;
        mesuresCumulees += v ;
        nombreMesuresValides += 1 ;
      }
    }
  }
  if (nombreMesuresValides > 0) {
    gErreur = 0 ;
    gMesure = (mesuresCumulees * 0.25) / nombreMesuresValides ;
  }else{
    gErreur = indicateursErreur ;
    gMesure = 2000.0 ;
    gNombreMesuresMoyennesInvalides += 1 ;
  }
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Obtenir le nombre de mesures incorrectes
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

uint32_t obtenirNombreMesuresBrutesIncorrectes (void) {
  return gNombreMesuresBrutesIncorrectes ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Obtenir le nombre de mesures incoherentes rejetees
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

uint32_t obtenirNombreMesuresBrutesIncoherentesRejetees (void) {
  return gNombreMesuresBrutesIncoherentesRejetees ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Obtenir le nombre de mesures moyennes invalides
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

uint32_t obtenirNombreMesuresMoyennesInvalides (void) {
  return gNombreMesuresMoyennesInvalides ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//  erreurCapteurTemperature (renvoie 0 si tout est ok)
// Seuls les trois bits de poids faible sont utilisés :
//      D2 : 0 --> ok, 1 --> SCV Fault : « the thermocouple is short-circuited to VCC »
//      D1 : 0 --> ok, 1 --> SCG Fault : « the thermocouple is short-circuited to GND »
//      D0 : 0 --> ok, 1 -->  OC Fault : « the thermocouple is open (no connections) »
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

uint32_t erreurCapteurTemperature (void) {
  return gErreur ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//                                    getTemp
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// This function returns the temperature in Celcius

double getTemp (void) {
 return gMesure ;

 //——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

}
