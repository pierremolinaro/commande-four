#include "Temp_Sensor.h"
#include "Defines.h"
#include "myAdafruit_MAX31855.h"

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//  Variables statiques
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

static myAdafruit_MAX31855 thermocouple (MAX31855_CLK, MAX31855_CS, MAX31855_DO) ;

static int32_t seqBin;

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
//  Configurer le thermo-couple
//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void configurerThermoCouple (void) {
  thermocouple.begin () ;
}

//——————————————————————————————————————————————————————————————————————————————————————————————————————————————————————

// ----------Functions----------
/*====================================================================================*
 *                                  updateTemp                                        *
 *====================================================================================*
 * This function stocks the data read by the thermocouple in a static variable seqBin.
 */
void updateTemp(void) {
    seqBin = thermocouple.rawValue ();
}

/*====================================================================================*
 *                                 testErrorTemp                                      *
 *====================================================================================*
 * This function returns true if there is an error in the data read.
 */
bool testErrorTemp(void) {
    return((seqBin & 0x7) > 0); // 3 last bits, if one is equal to 1, there is an error
}

/*====================================================================================*
 *                                    getTemp                                         *
 *====================================================================================*
 * This function returns the temperature read with the thermocouple.
 */
double getTemp(void) {
    int32_t seq = seqBin; // to copy the value, not to change seqBin
    if (seq & 0x80000000) { // if it starts by a 1
        // Negative value, drop the lower 18 bits and explicitly extend sign bits.
        seq = 0xFFFFC000 | (seq >> 18);
    }
    else {
        // Positive value, just drop the lower 18 bits.
        seq >>= 18;
    }
    double tempc = seq;
    return(tempc * 0.25); //last bit is equal to 0.25 °C
}
