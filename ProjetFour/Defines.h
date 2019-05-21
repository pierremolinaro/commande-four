// ----------Always begin by this pragma----------
#pragma once

#include <Arduino.h>

// ----------Constants declaration----------

//--- Canaux PWM (0 à 15)
const uint8_t PWM_CHANNEL_BACKLIGHT = 0 ;

//--- Numéro du timer (entre 0 et 3)
const uint8_t TIMER_ROTARY_ENCODER = 0 ;
const uint8_t TIMER_TEMPERATURE_SENSOR = 1 ;

//--- Rotary encoder pin declarations
const uint8_t PIN_ENCODER_A     = 35;
const uint8_t PIN_ENCODER_B     = 34;
const uint8_t PIN_ENCODER_CLICK = 39 ;

// LEDs declaration
const uint8_t LED_FOUR_CHAUD  =  2 ;  // marquée L1 sur la sérigraphie

//--- Led en marche (clignote à 1 Hz pour indiquer qu'un programme s'exécute)
const uint8_t LED_EN_MARCHE   = 15 ; // TDO, marquée L2 sur la sérigraphie

//--- TFT RESET
const uint8_t TFT_RESET = 25 ;

//--- TFT Backlight
const uint8_t PIN_BACKLIGHT = 32 ; // Contrôlé par PWM
const uint32_t BACKLIGHT_FREQUENCY = 5000 ; // Hz
const uint32_t BACKLIGHT_RESOLUTION = 12 ; // En bits

// Button declaration
const uint8_t BUTTON_ACTIVATE_BACKLIGHT  = 36; // SVP

// Relay declaration
const uint8_t PIN_OVEN_RELAY = 27 ;

// Creating a thermocouple instance with software SPI on any three digital IO pins
const uint8_t MAX31855_DO  = 17 ;
const uint8_t MAX31855_CS  =  4 ;
const uint8_t MAX31855_CLK = 16 ;

// SDCARD pins (codé en dur dans la librairie SD) : 
//    SDCARD_SCK  --> 14 (TMS)
//    SDCARD_MISO --> 12 (TDI) Attention TDI doit être à 0 lors du flashage (sinon échec) --> Enlever la carte SD
//    SDCARD_MOSI --> 13 (TCK)
const uint8_t SDCARD_CS = 26 ;
const uint8_t SDCARD_CD = 33 ;

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

//------------------- Caractères accentues 
#define DEGREE_CHAR        "\xF7"
#define LOWERCASE_E_ACUTE  "\x82"
#define LOWERCASE_E_CIRCUM "\x88"
