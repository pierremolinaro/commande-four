// ----------Always begin by this pragma----------
#pragma once

#include <Arduino.h>

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//  Initialiser le thermo-couple
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void initTemperatureSensor (void) ;

// ----------Functions declaration----------
/*====================================================================================*
 *                                  updateTemp                                        *
 *====================================================================================*
 * This function stocks the data read by the thermocouple in a static variable seqBin.
 */
void updateTemp(void);

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//  erreurCapteurTemperature (renvoie 0 si tout est ok)
// Seuls les trois bits de poids faible sont utilisés :
//      D2 : 0 --> ok, 1 --> SCV Fault : « the thermocouple is short-circuited to VCC »
//      D1 : 0 --> ok, 1 --> SCG Fault : « the thermocouple is short-circuited to GND »
//      D0 : 0 --> ok, 1 -->  OC Fault : « the thermocouple is open (no connections) »
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

const uint32_t ERREUR_CAPTEUR_ABSENT = 0x01 ;
const uint32_t ERREUR_CAPTEUR_COURT_CIRCUIT_GND = 0x02 ;
const uint32_t ERREUR_CAPTEUR_COURT_CIRCUIT_VCC = 0x04 ;

uint32_t erreurCapteurTemperature (void) ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Obtenir le nombre de mesures brutes incorrectes
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

uint32_t obtenirNombreMesuresBrutesIncorrectes (void) ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Obtenir le nombre de mesures brutes incohérentes rejetées
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

uint32_t obtenirNombreMesuresBrutesIncoherentesRejetees (void) ;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
// Obtenir le nombre de mesures moyennes invalides
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

uint32_t obtenirNombreMesuresMoyennesInvalides (void) ;

/*====================================================================================*
 *                                    getTemp                                         *
 *====================================================================================*
 * This function returns the temperature read with the thermocouple.
 */
double getTemp(void);
