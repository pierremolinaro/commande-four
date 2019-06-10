#include "MinFreeHeap.h"
#include "gcc-diagnostics.h"

//----------------------------------------------------------------------------------------------------------------------

static uint32_t gMinFreeHeap = UINT32_MAX ;

//----------------------------------------------------------------------------------------------------------------------

uint32_t minFreeHeap (void) {
  const uint32_t freeHeap = ESP.getFreeHeap () ;
  if (gMinFreeHeap > freeHeap) {
    gMinFreeHeap = freeHeap ;
  }
  return gMinFreeHeap ;
}

//----------------------------------------------------------------------------------------------------------------------
