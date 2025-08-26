#include <msp430.h>
//#include <cstdio>
#include "bluetooth.h"
/*
void init_gpio_uart_pins(void);
void init_clock(void);
void init_uart(void);
void uart_send_char(char c);
void uart_send_string(const char *str);
void init_bluetooth(void);
*/

void init_gpio_uart_pins(void) {
    // Configure P6.0 and P6.1 for UCA3 (UART TX/RX)
    P6SEL1 &= ~(BIT0 | BIT1);
    P6SEL0 |=  (BIT0 | BIT1);

    // Disable GPIO high-impedance mode (required for FRAM devices)
    PM5CTL0 &= ~LOCKLPM5;
}

void init_clock(void) {
    // Configure DCO to 8 MHz
    CSCTL0_H = CSKEY_H;                    // Unlock CS registers
    CSCTL1 = DCOFSEL_3 | DCORSEL;          // Set DCO = 8 MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK; // ACLK = VLO, SMCLK = MCLK = DCO
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;   // No clock division
    CSCTL0_H = 0;                          // Lock CS registers
}

void init_uart(void) {
    UCA3CTLW0 = UCSWRST;                  // Put UART in reset
    UCA3CTLW0 |= UCSSEL__SMCLK;           // Use SMCLK as clock source

    // Set baud rate to 9600 for 8 MHz SMCLK
    UCA3BRW = 52;                         // 8000000 / (16 * 9600) = 52.08
    UCA3MCTLW = UCOS16 | UCBRF_1 | 0x4900; // Oversampling, modulation settings

    UCA3CTLW0 &= ~UCSWRST;                // Release UART from reset
}

void uart_send_char(char c) {
    while (!(UCA3IFG & UCTXIFG));         // Wait for TX buffer ready
    UCA3TXBUF = c;                        // Send character
}

void uart_send_string(const char *str) {
    while (*str) {
        uart_send_char(*str++);
    }
}

#define CPU_FREQUENCY_HZ 1000000UL

void init_bluetooth(void){
    init_gpio_uart_pins();
    init_clock();
    init_uart();
}

/*
int main(void) {
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    init_gpio_uart_pins();
    init_clock();
    init_uart();

    const char *message = "Hello from MSP430!\r\n";
    while (1) {
        // Transmit each character
        for (const char *p = message; *p != '\0'; ++p) {
            while (!(UCA3IFG & UCTXIFG)) {
                ; // wait for TX buffer to be ready
            }
            UCA3TXBUF = *p;  // send next character
        }

        __delay_cycles(800000);  // delay ~0.1s (at 8 MHz) before sending again
    }
}
*/
/*
int main(void) {
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;  

    // Configure pins P6.0 and P6.1 for UCA3 UART (TXD/RXD)
    P6SEL1 &= ~(BIT0 | BIT1);
    P6SEL0 |=  (BIT0 | BIT1);   // Set P6.0, P6.1 to UART mode (UCA3 TX/RX)&#8203;:contentReference[oaicite:8]{index=8}

    // Unlock pins (disable high-impedance mode on FRAM devices)
    PM5CTL0 &= ~LOCKLPM5;      // Enable previously configured port settings&#8203;:contentReference[oaicite:9]{index=9}

    // Configure DCO clock to 8 MHz (for a stable UART baud rate source)
    CSCTL0_H = CSKEY_H;                   // Unlock CS registers
    CSCTL1 = DCOFSEL_3 | DCORSEL;         // Set DCO = 8 MHz&#8203;:contentReference[oaicite:10]{index=10}
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;  // ACLK = VLO, SMCLK = MCLK = DCO
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1; // Divide all clocks by 1 (no division)
    CSCTL0_H = 0;                        // Lock CS registers

    // Initialize eUSCI_A3 UART
    UCA3CTLW0 = UCSWRST;                 // Put UART in reset (to configure)
    UCA3CTLW0 |= UCSSEL__SMCLK;          // Use SMCLK as source (8 MHz)&#8203;:contentReference[oaicite:11]{index=11}

    // Baud rate configuration for 9600 with 8 MHz SMCLK
    // 8000000/(16*9600) ≈ 52.083 -> UCA3BRW = 52
    // Fractional part 0.083 * 16 = 1.33 -> UCBRF = 1, UCBRS ~ 0x49 (from table)&#8203;:contentReference[oaicite:12]{index=12}
    UCA3BRW = 52;                        // 8000000/(16*9600) = 52.083 -> BRW = 52&#8203;:contentReference[oaicite:13]{index=13}
    UCA3MCTLW = UCOS16 | UCBRF_1 | 0x4900; // Enable oversampling, set first mod stage =1, second mod =0x49&#8203;:contentReference[oaicite:14]{index=14}

    UCA3CTLW0 &= ~UCSWRST;               // Take UART out of reset, UART is now live

    // Main loop: send the message repeatedly
    const char *message = "Hello from MSP430!\r\n";
    while (1) {
        // Transmit each character
        for (const char *p = message; *p != '\0'; ++p) {
            while (!(UCA3IFG & UCTXIFG)) {
                ; // wait for TX buffer to be ready
            }
            UCA3TXBUF = *p;  // send next character
        }

        __delay_cycles(800000);  // delay ~0.1s (at 8 MHz) before sending again
    }
}
*/
