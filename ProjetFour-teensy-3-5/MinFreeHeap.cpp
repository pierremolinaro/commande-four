#include "MinFreeHeap.h"
#include "gcc-diagnostics.h"

//----------------------------------------------------------------------------------------------------------------------

static uint32_t gMinFreeHeap = UINT32_MAX ;

extern "C" char* sbrk (int incr);

//----------------------------------------------------------------------------------------------------------------------

uint32_t minFreeHeap (void) {
  uint32_t top; // For getting stack address
  const uint32_t freeHeap = ((size_t) &top) - (size_t) sbrk (0) ;
  if (gMinFreeHeap > freeHeap) {
    gMinFreeHeap = freeHeap ;
  }
  return gMinFreeHeap ;
}

//----------------------------------------------------------------------------------------------------------------------
