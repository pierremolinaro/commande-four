//
//  main.cpp
//  tableau-correction-MAX31855
//
//  Created by Pierre Molinaro on 28/07/2019.
//  Copyright © 2019 Pierre Molinaro. All rights reserved.
//

#include "TemperatureCorrection.h"
#include <iostream>

int main(int argc, const char * argv[]) {
  const double junctionTemp = 30.0 ;
  for (double rawTemp = -20.0 ; rawTemp < 1100.0 ; rawTemp += 1.0) {
    const double t = correctedCelsius (rawTemp, junctionTemp) ;
    printf (" raw %g -> %g\n", rawTemp, t) ;
  }

  return 0;
}
