#include <msp430.h>
#include <stdint.h>
#include "serialmonitor.h"

//Make commands
//make install_myoware
//screen /dev/ttyACM1 9600
//To leave screen press CNTRL+A then k then y

void initADC() {
    // Configure P3.0 for ADC (MyoWare SIG connected here)
    P3SEL1 |= BIT0;
    P3SEL0 |= BIT0;

    // Disable ADC before configuration
    ADC12CTL0 &= ~ADC12ENC; 

    // Configure ADC12 for single-channel sampling
    ADC12CTL0 = ADC12SHT0_2 | ADC12ON; // Sample & hold time = 16 cycles, ADC enabled
    ADC12CTL1 = ADC12SHP | ADC12SSEL_3; // Use sampling timer, SMCLK as ADC clock source
    ADC12CTL2 |= ADC12RES_2;  // 12-bit resolution
    ADC12MCTL0 = ADC12INCH_12; // Select input channel A12 (P3.0)

    // Enable ADC after configuration
    ADC12CTL0 |= ADC12ENC;
}

uint16_t readADC() {
    ADC12CTL0 |= ADC12SC; // Start conversion

    // Wait for conversion to complete
    while (ADC12CTL1 & ADC12BUSY);

    return ADC12MEM0; // Return ADC result
}

/*
int main(void) {
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5; // Unlock GPIO (for newer MSP430s with LPM)

    configureClockAndUART();  // Setup UART
    initADC();                // Setup ADC

    while (1) {
        uint16_t emg_signal = readADC(); // Read MyoWare signal

        printNum(emg_signal); // Send ADC value over UART
        printStr("\r\n");       // New line for readability

        __delay_cycles(50000); // Small delay
    }
}
    */
