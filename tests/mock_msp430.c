#ifdef HOST_BUILD
#include <stdint.h>

////////////////////////////////////////////////////////////

//                   BLUETOOTH MOCK

////////////////////////////////////////////////////////////

/* ------------------- 8-bit Port Registers ------------------- */
volatile uint8_t P6SEL0;
volatile uint8_t P6SEL1;

/* ------------------- 16-bit System Registers ------------------- */
volatile uint16_t PM5CTL0;    // Power Management (LOCKLPM5, etc.)
volatile uint16_t CSCTL0_H;   // Clock System Control 0 (high byte)
volatile uint16_t CSCTL1;     // Clock System Control 1
volatile uint16_t CSCTL2;     // Clock System Control 2
volatile uint16_t CSCTL3;     // Clock System Control 3
volatile uint16_t UCA3CTLW0;  // eUSCI_A3 Control Word Register 0
volatile uint16_t UCA3BRW;    // eUSCI_A3 Baud Rate Control
volatile uint16_t UCA3MCTLW;  // eUSCI_A3 Modulation Control
volatile uint16_t UCA3IFG;    // eUSCI_A3 Interrupt Flags

/* UART transmit buffer (8-bit) */
volatile uint8_t UCA3TXBUF;

/* --------------- Common Bit Definitions & Constants --------------- */
#define BIT0               0x01
#define BIT1               0x02
#define LOCKLPM5           0x0001

// Clock System “password” or key
#define CSKEY_H            0xA500

// DCO frequency range bits
#define DCOFSEL_3          0x0001
#define DCORSEL            0x0002

// Clock source selects (just arbitrary bit patterns for stubbing)
#define SELA__VLOCLK       0x0004
#define SELS__DCOCLK       0x0008
#define SELM__DCOCLK       0x0010

// Clock dividers
#define DIVA__1            0x0020
#define DIVS__1            0x0040
#define DIVM__1            0x0080

// eUSCI_A Control bits
#define UCSWRST            0x0001      // Software reset
#define UCSEL__SMCLK       0x0002      // SMCLK as clock source
#define UCOS16             0x0004      // Oversampling mode
#define UCBRF_1            0x0008      // One of the baud rate fine-tune bits
#define UCBRS_1            0x0010      // Another baud rate bit if used

// eUSCI_A Interrupt bits (within UCA3IFG)
#define UCTXIFG            0x0002      // TX buffer ready flag, for example

/*
 * If your code uses CPU_FREQUENCY_HZ in a __delay_cycles() call,
 * you can define a default frequency here to avoid errors.
 */
#ifndef CPU_FREQUENCY_HZ
#define CPU_FREQUENCY_HZ   8000000UL
#endif

/*
 * Stub out __delay_cycles so it compiles & links on a PC.
 * We do nothing in this function because there's no real hardware timing.
 */
/*
void __delay_cycles(unsigned long cycles)
{
    (void)cycles; // suppress unused variable warning
    // no-op
}
*/

////////////////////////////////////////////////////////////

//                   LED MOCK

////////////////////////////////////////////////////////////


volatile uint8_t P2DIR = 0;
volatile uint8_t P2OUT = 0;
volatile uint8_t P4DIR = 0;
volatile uint8_t P4OUT = 0;


////////////////////////////////////////////////////////////

//                   Myoware MOCK

////////////////////////////////////////////////////////////


volatile uint8_t  P3SEL0 = 0;
volatile uint8_t  P3SEL1 = 0;
volatile uint16_t ADC12CTL0 = 0;
volatile uint16_t ADC12CTL1 = 0;
volatile uint16_t ADC12CTL2 = 0;
volatile uint16_t ADC12MCTL0 = 0;
volatile uint16_t ADC12MEM0 = 0;

////////////////////////////////////////////////////////////

//                   SerialMonitor MOCK

////////////////////////////////////////////////////////////


volatile uint8_t  P2SEL0     = 0;
volatile uint8_t  P2SEL1     = 0;
volatile uint16_t UCA0CTLW0  = 0;
volatile uint16_t UCA0BRW    = 0;
volatile uint16_t UCA0MCTLW  = 0;
volatile uint16_t UCA0STATW  = 0;
volatile uint8_t  UCA0RXBUF  = 0;
volatile uint8_t  UCA0TXBUF  = 0;

////////////////////////////////////////////////////////////

//                   LCD MOCK

////////////////////////////////////////////////////////////

volatile uint8_t P3DIR = 0;  // For LCD_RS, LCD_EN, LCD_D4, LCD_D5
volatile uint8_t P3OUT = 0;

volatile uint8_t P7DIR = 0;  // For LCD_D6
volatile uint8_t P7OUT = 0;

// -----------------------------------------------------------------
// Optional: Override __delay_cycles() for LCD timing tests
// -----------------------------------------------------------------
/*
static unsigned long mockDelayCycles = 0;
#undef __delay_cycles
#define __delay_cycles(n) (mockDelayCycles += (n))
*/


#endif /* HOST_BUILD */
