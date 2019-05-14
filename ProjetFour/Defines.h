// ----------Always begin by this pragma----------
#pragma once

#include <Arduino.h>

// ----------Constants declaration----------

//--- Canaux PWM (0 à 15)
const uint8_t PWMchannelBuzz =  0;
const uint8_t PWMchannelLED2 =  1;
const uint8_t CANAL_PWM_RETRO_ECLAIRAGE = 2 ;

// Rotary encoder declaration
const uint8_t pinA     = 35;
const uint8_t pinB     = 34;
const uint8_t clickPin = 39;

// LEDs declaration
const uint8_t LED_FOUR_CHAUD  =  2 ;

//--- Led en marche (clignote à 1 Hz pour indiquer qu'un programme s'exécute)
const uint8_t LED_EN_MARCHE   = 15 ; // TDO, contrôlé par PWM
const uint32_t PWMfrequency   =  1;
const uint8_t  PWMresolution  =  8; //Resolution 8, 10, 12, 15

//--- Retro-eclairage
const uint8_t RETRO_ECLAIRAGE = 32 ; // Contrôlé par PWM
const uint32_t FREQUENCE_RETRO_ECLAIRAGE = 5000 ; // Hz
const uint8_t RESOLUTION_RETRO_ECLAIRAGE = 12 ; // En bits

// Button declaration
const uint8_t BOUTON_BTN  = 36; // SVP

// Relay declaration
const uint8_t ovenRelay = 27;

// Buzzer declaration including a PWM
const uint8_t  buzzer         = 25;

// Creating a thermocouple instance with software SPI on any three digital IO pins
const uint8_t MAX31855_DO  = 17;
const uint8_t MAX31855_CS  =  4;
const uint8_t MAX31855_CLK = 16;

// SDCARD pin (codé en dur dans la librairie SD : 
//    SDCARD_SCK  --> 14 (TMS)
//    SDCARD_MISO --> 12 (TDI)
//    SDCARD_MOSI --> 13 (TCK)
const uint8_t SDCARD_CS = 26 ;

// Reading on the SD card declaration
const String curvesDir = "/Courbes";
const String valuesDir = "/ValeursCourbes";

// The maximal length of the curve name
const uint8_t maxlength = 18;

// The maximal number of different curves
const uint8_t maxnbCurves = 18;

// ----------Structures declaration----------
// Define a structure to stock values of temperature corresponding to a instant of time
struct TimeTemp {
    uint16_t Time;
    float    Temp;
};
