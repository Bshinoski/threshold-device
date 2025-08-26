#ifndef TIMER_H
#define TIMER_H

#define CLOCK_FREQUENCY 8000000  // 8 MHz
#define CYCLES_PER_MS (CLOCK_FREQUENCY / 1000)  // 8000 cycles per ms

// ------------------ Function Prototypes ------------------
void delayMs(unsigned int ms);
void delayMsTimer(unsigned int ms);

#endif