// mock_delay_cycles.h
#ifndef MOCK_DELAY_CYCLES_H
#define MOCK_DELAY_CYCLES_H

#ifdef HOST_BUILD

extern unsigned long mockDelayCycles;
#define __delay_cycles(n) (mockDelayCycles += (n))

#endif

#endif