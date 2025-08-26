#include "msp430.h"
#include "timer.h"

#ifdef HOST_BUILD
#include "../../tests/mock_delay_cycles.h"
#endif

// ------------------------------------------------------
// Simple Delay (ms) – unchanged from your typical 1MHz setup
//------------------------------------------------------
void delayMs(unsigned int ms) {
    unsigned int i;
    for (i = 0; i < ms; i++) {
        __delay_cycles(1000);
    }
}
void delayMsTimer(unsigned int ms) {
    unsigned int i;
    for (i = 0; i < ms; i++) {
        __delay_cycles(CYCLES_PER_MS);
    }
}
