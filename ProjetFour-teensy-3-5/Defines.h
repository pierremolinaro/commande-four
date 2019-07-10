#pragma once

#include "check-board.h"

#include <Arduino.h>

//----------------------------------------------------------------------------------------------------------------------

//--- Rotary encoder pin declarations
const uint8_t PIN_ENCODER_A     = 35 ;
const uint8_t PIN_ENCODER_B     = 36 ;
const uint8_t PIN_ENCODER_CLICK = 34 ;

// LEDs declaration
const uint8_t LED_HOT_OVEN = 33 ;  // marquée L1 sur la sérigraphie

//--- Led en marche (clignote à 1 Hz pour indiquer qu'un programme s'exécute)
const uint8_t LED_EN_MARCHE   = 39 ; // marquée L2 sur la sérigraphie

//--- TFT
const uint8_t TFT_CS    = 7 ;
const uint8_t TFT_DC    = 6 ;
const uint8_t TFT_RESET = 8 ;
const uint8_t TFT_MOSI  = 0 ;
const uint8_t TFT_SCK  = 32 ;
const uint8_t TFT_MISO  = 1 ;

//--- TFT Backlight
const uint8_t PIN_BACKLIGHT = 23 ; // Contrôlé par PWM
const uint32_t BACKLIGHT_FREQUENCY = 5000 ; // Hz
const uint32_t BACKLIGHT_RESOLUTION = 12 ; // En bits

// Button declaration
const uint8_t BUTTON_ACTIVATE_BACKLIGHT  = 27 ;

// Relay declaration
const uint8_t PIN_OVEN_RELAY = 24 ;

// Creating a thermocouple instance with software SPI on any three digital IO pins
const uint8_t MAX31855_DO  = 17 ;
const uint8_t MAX31855_CS  = 15 ;
const uint8_t MAX31855_CLK = 16 ;

// SDCARD pins :
const uint8_t SDCARD_SCK = 14 ;
//    SDCARD_MISO --> 12
//    SDCARD_MOSI --> 11
const uint8_t SDCARD_CS = 10 ;
const uint8_t SDCARD_CD = 28 ;

//----------------------------------------------------------------------------------------------------------------------
//  SD CARD
//----------------------------------------------------------------------------------------------------------------------

extern const char * const PROFILES_DIRECTORY ; // Defined in Defines.cpp
extern const char * const RESULT_DIRECTORY   ; // Defined in Defines.cpp
const uint32_t FILE_BASE_NAME_MAX_LENGTH = 20 ; // Without path, without .csv extension

const uint32_t PROGRAM_POINT_MAX_COUNT = 50 ;

//----------------------------------------------------------------------------------------------------------------------
//  ACCENTED CHARACTERS
//----------------------------------------------------------------------------------------------------------------------

#define DEGREE_CHAR        "\xF7"
#define LOWERCASE_E_ACUTE  "\x82"
#define LOWERCASE_A_ACUTE  "\x85"
#define LOWERCASE_E_CIRCUM "\x88"

//----------------------------------------------------------------------------------------------------------------------
