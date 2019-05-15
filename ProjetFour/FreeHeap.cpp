#include "FreeHeap.h"

static uint32_t gMinRamLibre = UINT32_MAX ;

uint32_t ramLibre (void) {
  const uint32_t freeRAM = ESP.getFreeHeap () ;
  if (gMinRamLibre > freeRAM) {
    gMinRamLibre = freeRAM ;
  }
  return gMinRamLibre ;
}
