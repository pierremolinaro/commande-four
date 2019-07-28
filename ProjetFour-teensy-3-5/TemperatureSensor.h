#pragma once

//----------------------------------------------------------------------------------------------------------------------
//  Check board type
//----------------------------------------------------------------------------------------------------------------------

#include "check-board.h"

//----------------------------------------------------------------------------------------------------------------------

#include <Arduino.h>

//----------------------------------------------------------------------------------------------------------------------
//  INIT
//----------------------------------------------------------------------------------------------------------------------

void initTemperatureSensor (void) ;

// ----------Functions declaration----------
/*====================================================================================*
 *                                  updateTemp                                        *
 *====================================================================================*
 * This function stocks the data read by the thermocouple in a static variable seqBin.
 */
void updateTemp(void);

//----------------------------------------------------------------------------------------------------------------------
//  erreurCapteurTemperature (renvoie 0 si tout est ok)
// Seuls les trois bits de poids faible sont utilisés :
//      D2 : 0 --> ok, 1 --> SCV Fault : « the thermocouple is short-circuited to VCC »
//      D1 : 0 --> ok, 1 --> SCG Fault : « the thermocouple is short-circuited to GND »
//      D0 : 0 --> ok, 1 -->  OC Fault : « the thermocouple is open (no connections) »
//----------------------------------------------------------------------------------------------------------------------

const uint32_t TEMPERATURE_SENSOR_ERROR_NO_CONNECTION          = 0x01 ;
const uint32_t TEMPERATURE_SENSOR_ERROR_SHORT_CIRCUITED_TO_GND = 0x02 ;
const uint32_t TEMPERATURE_SENSOR_ERROR_SHORT_CIRCUITED_TO_VCC = 0x04 ;

uint32_t temperatureSensorErrorFlags (void) ;

//----------------------------------------------------------------------------------------------------------------------
// Obtenir le nombre de mesures brutes incorrectes
//----------------------------------------------------------------------------------------------------------------------

uint32_t getFaultlySampleCount (void) ;

//----------------------------------------------------------------------------------------------------------------------
// Obtenir le nombre de mesures brutes incohérentes rejetées
//----------------------------------------------------------------------------------------------------------------------

uint32_t getRejectedInconsistentSampleCount (void) ;

//----------------------------------------------------------------------------------------------------------------------
// Obtenir le nombre de mesures moyennes invalides
//----------------------------------------------------------------------------------------------------------------------

uint32_t obtenirNombreMesuresMoyennesInvalides (void) ;

//----------------------------------------------------------------------------------------------------------------------
// GET SENSOR TEMPERATURE (Celcius)
//----------------------------------------------------------------------------------------------------------------------

double getSensorTemperature (void) ; // Corrected temperature


double getRawTemperature (void) ; // Used only for information


double getJunctionTemperature (void) ; // Used only for information

//----------------------------------------------------------------------------------------------------------------------

void runTemperatureSensorFromISR (void) ;
