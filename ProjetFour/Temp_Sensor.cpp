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

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//  Variables statiques
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static uint32_t gRawValue;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//  Configurer le thermo-couple
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void initialiserThermoCouple (void) {
  pinMode (MAX31855_CS, OUTPUT) ;
  digitalWrite (MAX31855_CS, HIGH) ;
  pinMode (MAX31855_CLK, OUTPUT) ;
  digitalWrite (MAX31855_CLK, HIGH) ;
  pinMode (MAX31855_DO, INPUT_PULLUP) ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//  updateTemp
// This function writes the data read from the thermocouple in a static variable gRawValue.
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void updateTemp (void) {
  gRawValue = 0 ;

  digitalWrite (MAX31855_CS, LOW) ;
  delayMicroseconds (1) ;

  for (int32_t i = 31 ; i >= 0 ; i--) {
    digitalWrite (MAX31855_CLK, LOW);
    delayMicroseconds (1) ;
    gRawValue <<= 1 ;
    if (digitalRead (MAX31855_DO)) {
      gRawValue |= 1 ;
    }

    digitalWrite (MAX31855_CLK, HIGH) ;
    delayMicroseconds (1) ;
  }

  digitalWrite (MAX31855_CS, HIGH) ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//  erreurCapteurTemperature (renvoie 0 si tout est ok)
// Seuls les trois bits de poids faible sont utilisés :
//      D2 : 0 --> ok, 1 --> SCV Fault : « the thermocouple is short-circuited to VCC »
//      D1 : 0 --> ok, 1 --> SCG Fault : « the thermocouple is short-circuited to GND »
//      D0 : 0 --> ok, 1 -->  OC Fault : « the thermocouple is open (no connections) »
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

uint32_t erreurCapteurTemperature (void) {
  return gRawValue & 0x07 ;
}

/*====================================================================================*
 *                                    getTemp                                         *
 *====================================================================================*
 * This function returns the temperature read with the thermocouple.
 */
double getTemp (void) {
    int32_t seq = (int32_t) gRawValue; // to copy the value, not to change gRawValue
    if (seq & 0x80000000) { // if it starts by a 1
        // Negative value, drop the lower 18 bits and explicitly extend sign bits.
        seq = 0xFFFFC000 | (seq >> 18);
    }
    else {
        // Positive value, just drop the lower 18 bits.
        seq >>= 18;
    }
    double tempc = seq;
    return(tempc * 0.25); //last bit is equal to 0.25 °C
}
