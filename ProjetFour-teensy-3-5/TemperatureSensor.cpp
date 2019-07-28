//----------------------------------------------------------------------------------------------------------------------
//  INCLUDES
//----------------------------------------------------------------------------------------------------------------------

#include "TemperatureSensor.h"
#include "Defines.h"
#include "gcc-diagnostics.h"
#include "TemperatureCorrection.h"

//----------------------------------------------------------------------------------------------------------------------
//  CONSTANTS
//----------------------------------------------------------------------------------------------------------------------

static const uint32_t ACQUISITION_PERIOD = 125000 ; // In micro-seconds
static const uint32_t SAMPLE_ARRAY_SIZE = 8 ;

//----------------------------------------------------------------------------------------------------------------------
// La lecture par (soft) SPI renvoie une valeur sur 32 bits dont la composition est la suivante :
//      D31 ... D18 : 14 bits signés donnant la température en quart de Celcius
//      D17 : réservé, toujours lu à 0
//      D16 : 1 --> faute, 0 --> ok
//      D15 ... D4 : 12 bits donnant la température interne (inutilisés)
//      D3 : réservé, toujours lu à 0
//      D2 : 0 --> ok, 1 --> SCV Fault : « the thermocouple is short-circuited to VCC »
//      D1 : 0 --> ok, 1 --> SCG Fault : « the thermocouple is short-circuited to GND »
//      D0 : 0 --> ok, 1 -->  OC Fault : « the thermocouple is open (no connections) »
//----------------------------------------------------------------------------------------------------------------------
//  STATIC VARIABLES
//----------------------------------------------------------------------------------------------------------------------

static volatile uint32_t gSampleArray [SAMPLE_ARRAY_SIZE] ;
static uint32_t gSampleArrayIndex ; // Entre 0 et SAMPLE_ARRAY_SIZE - 1

static volatile uint32_t gRejectedInconsistentSampleCount ;
static volatile uint32_t gFaultlySampleCount ;
static uint32_t gNombreMesuresMoyennesInvalides ;

static uint32_t gErrorFlags ;
static double gRawTemperature ;
static double gJunctionTemperature ;
static double gCorrectedTemperature ;

//----------------------------------------------------------------------------------------------------------------------

void runTemperatureSensorFromISR (void) {
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
  const uint32_t acceptationMask = (1 << 3) | (1 << 17) ;
  if ((rawValue & acceptationMask) != 0) {
    gRejectedInconsistentSampleCount += 1 ;
    rawValue = UINT32_MAX ; // Indiquer une valeur incohérente
  }else if (temperatureSensorErrorFlags () != 0) { //--- Comptabiliser le nombre de mesures incorrectes
    gFaultlySampleCount += 1 ;
  }
//--- Entrer la mesure dans le tableau
  gSampleArray [gSampleArrayIndex] = rawValue ;
  gSampleArrayIndex += 1 ;
  if (gSampleArrayIndex == SAMPLE_ARRAY_SIZE) {
    gSampleArrayIndex = 0 ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
//  Configurer le thermo-couple
//----------------------------------------------------------------------------------------------------------------------

void initTemperatureSensor (void) {
  pinMode (MAX31855_CS, OUTPUT) ;
  digitalWrite (MAX31855_CS, HIGH) ;
  pinMode (MAX31855_CLK, OUTPUT) ;
  digitalWrite (MAX31855_CLK, HIGH) ;
  pinMode (MAX31855_DO, INPUT_PULLUP) ;
}

//----------------------------------------------------------------------------------------------------------------------
// Obtenir le nombre de mesures incorrectes
//----------------------------------------------------------------------------------------------------------------------

uint32_t getFaultlySampleCount (void) {
  return gFaultlySampleCount ;
}

//----------------------------------------------------------------------------------------------------------------------
// Obtenir le nombre de mesures incoherentes rejetees
//----------------------------------------------------------------------------------------------------------------------

uint32_t getRejectedInconsistentSampleCount (void) {
  return gRejectedInconsistentSampleCount ;
}

//----------------------------------------------------------------------------------------------------------------------
// Obtenir le nombre de mesures moyennes invalides
//----------------------------------------------------------------------------------------------------------------------

uint32_t obtenirNombreMesuresMoyennesInvalides (void) {
  return gNombreMesuresMoyennesInvalides ;
}

//----------------------------------------------------------------------------------------------------------------------
//  updateTemp
// This function writes the data read from the thermocouple in a static variable gRawValue.
//----------------------------------------------------------------------------------------------------------------------

void updateTemp (void) {
  int32_t nombreMesuresValides = 0 ;
  uint32_t errorFlags = 0 ; // 0 -> No error
  int32_t mesuresCumulees = 0 ;
  int32_t sumOfJunctionTemperatures = 0 ;
  for (uint32_t i=0 ; i<SAMPLE_ARRAY_SIZE ; i++) {
    const uint32_t mesure = gSampleArray [i] ;
    if (mesure != UINT32_MAX) { // Mesure incohérente rejetée
      const uint32_t erreur = mesure & 0x07 ;
      errorFlags |= erreur ;
      if (erreur == 0) {
        int32_t v = (int32_t) mesure ;
        v >>= 18 ;
        mesuresCumulees += v ;
        v = (int32_t) mesure ;
        v <<= 16 ;
        v >>= (16 + 4) ;
        sumOfJunctionTemperatures += v ;
        nombreMesuresValides += 1 ;
      }
    }
  }
  if (nombreMesuresValides > 0) {
    gErrorFlags = 0 ;
    gRawTemperature = (((double) mesuresCumulees) * 0.25) / (double) nombreMesuresValides ;
    gJunctionTemperature = (((double) sumOfJunctionTemperatures) / 16.0) / (double) nombreMesuresValides ;
    gCorrectedTemperature = correctedCelsius (gRawTemperature, gJunctionTemperature) ;
  }else{
    gErrorFlags = errorFlags ;
    gRawTemperature = 2000.0 ;
    gCorrectedTemperature = 2000.0 ;
    gJunctionTemperature = 128.0 ;
    gNombreMesuresMoyennesInvalides += 1 ;
  }
}

//----------------------------------------------------------------------------------------------------------------------
//  erreurCapteurTemperature (renvoie 0 si tout est ok)
// Seuls les trois bits de poids faible sont utilisés :
//      D2 : 0 --> ok, 1 --> SCV Fault : « the thermocouple is short-circuited to VCC »
//      D1 : 0 --> ok, 1 --> SCG Fault : « the thermocouple is short-circuited to GND »
//      D0 : 0 --> ok, 1 -->  OC Fault : « the thermocouple is open (no connections) »
//----------------------------------------------------------------------------------------------------------------------

uint32_t temperatureSensorErrorFlags (void) {
  return gErrorFlags ;
}

//----------------------------------------------------------------------------------------------------------------------
//    getSensorTemperature
//----------------------------------------------------------------------------------------------------------------------

double getSensorTemperature (void) {
  return gCorrectedTemperature ;
}

//----------------------------------------------------------------------------------------------------------------------

double getRawTemperature (void) {
  return gRawTemperature ;
}

//----------------------------------------------------------------------------------------------------------------------

double getJunctionTemperature (void) {
  return gJunctionTemperature ;
}

//----------------------------------------------------------------------------------------------------------------------
