// ----------Always begin by this pragma----------
#pragma once

// ----------Constants declaration----------
// Rotary encoder declaration
const uint8_t pinA     = 35;
const uint8_t pinB     = 34;
const uint8_t clickPin = 39;

// LEDs declaration
const uint8_t LED1 = 32;
const uint8_t LED2 = 26;

// Button declaration
const uint8_t Btn  = 36; // SVP

// Relay declaration
const uint8_t ovenRelay = 27;

// Buzzer declaration including a PWM
const uint8_t  buzzer         = 25;
const uint8_t  PWMchannelBuzz =  0;
const uint8_t  PWMchannelLED2 =  1;
const uint32_t PWMfrequency   =  1;
const uint8_t  PWMresolution  =  8; //Resolution 8, 10, 12, 15

// Creating a thermocouple instance with software SPI on any three digital IO pins
const uint8_t MAXDO  = 17;
const uint8_t MAXCS  =  4;
const uint8_t MAXCLK = 16;

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
