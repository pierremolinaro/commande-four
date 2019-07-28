// http://forums.adafruit.com/viewtopic.php?f=19&t=32086&p=372992#p372992

// https://srdata.nist.gov/its90/type_k/kcoefficients.html
// https://srdata.nist.gov/its90/download/type_k.tab
// https://learn.adafruit.com/calibrating-sensors/maxim-31855-linearization
// The first code (from jh421797) is WRONG, it does not handle thermocoupleVoltage < 0
//----------------------------------------------------------------------------------------------------------------------

#include <math.h>
#include "TemperatureCorrection.h"

//----------------------------------------------------------------------------------------------------------------------

static double coldJunctionEquivalentThermocoupleVoltage (const double inColdJunctionTemperature) {
  if (inColdJunctionTemperature < 0.0) {
    const int N = 11 ;
    const double c [N] = {
       0.000000000000E+00,
       0.394501280250E-01,
       0.236223735980E-04,
      -0.328589067840E-06,
      -0.499048287770E-08,
      -0.675090591730E-10,
      -0.574103274280E-12,
      -0.310888728940E-14,
      -0.104516093650E-16,
      -0.198892668780E-19,
      -0.163226974860E-22
    } ;
    double coldJunctionVoltage = 0.0 ;
    for (int j = N-1 ; j >= 0 ; j--) {
      coldJunctionVoltage = c[j] + coldJunctionVoltage * inColdJunctionTemperature ;
    }
    return coldJunctionVoltage ;
  }else{
    const int N = 10 ;
    const double c [N] = {
      -0.176004136860E-01,
       0.389212049750E-01,
       0.185587700320E-04,
      -0.994575928740E-07,
       0.318409457190E-09,
      -0.560728448890E-12,
       0.560750590590E-15,
      -0.320207200030E-18,
       0.971511471520E-22,
      -0.121047212750E-25
    } ;
    const double a0 =  0.118597600000E+00 ;
    const double a1 = -0.118343200000E-03 ;
    const double a2 =  0.126968600000E+03 ;
    double coldJunctionVoltage = 0.0 ;
    for (int j = N-1 ; j >= 0 ; j--) {
      coldJunctionVoltage = c[j] + coldJunctionVoltage * inColdJunctionTemperature ;
    }
    coldJunctionVoltage += a0 * exp (a1 * pow ((inColdJunctionTemperature - a2), 2.0)) ;
    return coldJunctionVoltage ;
  }
}

//----------------------------------------------------------------------------------------------------------------------

double correctedCelsius (const double inRawTemperatureValue, const double inColdJunctionTemperature) {
//--- MAX31855 thermocouple voltage reading in mV
  const double thermocoupleVoltage = (inRawTemperatureValue - inColdJunctionTemperature) * 0.041276 ;
//--- MAX31855 cold junction voltage reading in mV
  const double coldJunctionVoltage = coldJunctionEquivalentThermocoupleVoltage (inColdJunctionTemperature) ;
  const double totalVoltage = thermocoupleVoltage + coldJunctionVoltage ;
//--- Calculate corrected temperature reading based on coefficients for 3 different ranges
  double b0, b1, b2, b3, b4, b5, b6, b7, b8, b9 ;
  if (totalVoltage < 0.0) {
    b0 =  0.0000000E+00;
    b1 =  2.5173462E+01;
    b2 = -1.1662878E+00;
    b3 = -1.0833638E+00;
    b4 = -8.9773540E-01;
    b5 = -3.7342377E-01;
    b6 = -8.6632643E-02;
    b7 = -1.0450598E-02;
    b8 = -5.1920577E-04;
    b9 =  0.0000000E+00;
  }else if (totalVoltage < 20.644) {
    b0 =  0.000000E+00;
    b1 =  2.508355E+01;
    b2 =  7.860106E-02;
    b3 = -2.503131E-01;
    b4 =  8.315270E-02;
    b5 = -1.228034E-02;
    b6 =  9.804036E-04;
    b7 = -4.413030E-05;
    b8 =  1.057734E-06;
    b9 = -1.052755E-08;
  }else if (totalVoltage < 54.886) {
    b0 = -1.318058E+02;
    b1 =  4.830222E+01;
    b2 = -1.646031E+00;
    b3 =  5.464731E-02;
    b4 = -9.650715E-04;
    b5 =  8.802193E-06;
    b6 = -3.110810E-08;
    b7 =  0.000000E+00;
    b8 =  0.000000E+00;
    b9 =  0.000000E+00;
  }else{
    return 2000.0 ; // Out of range...
  }
  double r = b8 + b9 * totalVoltage ;
  r = b7 + r * totalVoltage ;
  r = b6 + r * totalVoltage ;
  r = b5 + r * totalVoltage ;
  r = b4 + r * totalVoltage ;
  r = b3 + r * totalVoltage ;
  r = b2 + r * totalVoltage ;
  r = b1 + r * totalVoltage ;
  r = b0 + r * totalVoltage ;
  return r ;
}

//----------------------------------------------------------------------------------------------------------------------
