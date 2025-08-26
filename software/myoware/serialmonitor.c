/*****************************************************************
 Author: Simeon Babatunde and Arwa Alsubi
 Date: January 7, 2019
 Purpose:
 This library was written to enable easy debugging of Embedded C
 programs on TI MSP430Fr5994. It allows you to print string, number,
 or content of variables on a screen/terminal using printStr("foo") or
 printNum(500) statements.
 Setup:
 1. Copy both serialmonitor.h and serialmonitor.c into your project dir. and
    include serialmonitor.h in your main.c. Then call the configureClockAndUART();
    function in the main setup.
 2. Update your makefile with the serialmonitor information or use the sample
    makefile here.
 3. On your teminal, install the screen module using : sudo apt install screen
 4. Connect your MSP430 to your laptop and check the port using: ls /dev/,
    many open ports will show up but look out for ttyACM1 (Linux user) or
    cu.usbmodemxxxx (Mac user), where xxxx is a number, choose the second port.
 5. Establish a connection using : screen /dev/ttyACM1 9600 or
    screen /dev/cu.usbmodemxxxx 9600

 Hurray! you can now print values in your program and see them on the terminal,
 just like would in Energia.
*****************************************************************/

#include "serialmonitor.h"

/****************************************************************
*FUNCTION NAME: configureClockAndUART()
*FUNCTION     : Allows user to set clock frequency(88MHz) and UART module (9600)
*INPUT        : none
*OUTPUT       : none
****************************************************************/
void configureClockAndUART()
{

  // Startup clock system with max DCO setting ~8MHz
  CSCTL0_H = CSKEY_H;                     // Unlock CS registers
  CSCTL1 = DCOFSEL_3 | DCORSEL;           // Set DCO to 8MHz
  CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
  CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;   // Set all dividers
  CSCTL0_H = 0;                           // Lock CS registers

  // Config pins for UART operation P2.0 -> TX , P2.1-> RX
  P2SEL0 &= ~(BIT0 | BIT1);
  P2SEL1 |= (BIT0 | BIT1);
  // Configure USCI_A0 for UART mode
  UCA0CTLW0 = UCSWRST;                    // Put eUSCI in reset
  UCA0CTLW0 |= UCSSEL__SMCLK;             // CLK = SMCLK
  // Baud Rate calculation
  // 8000000/(16*9600) = 52.083
  // Fractional portion = 0.083
  // User's Guide Table 21-4: UCBRSx = 0x04
  // UCBRFx = int ( (52.083-52)*16) = 1
  UCA0BRW = 52;                           // 8000000/16/9600
  UCA0MCTLW |= UCOS16 | UCBRF_1 | 0x4900;
  UCA0CTLW0 &= ~UCSWRST;                  // Initialize eUSCI
}


/****************************************************************
*FUNCTION NAME: printStr()
*FUNCTION     : Allows user to print sting to the screen
*INPUT        : String to be printed
*OUTPUT       : none
****************************************************************/
void printStr(const char *tx_data)
{
  unsigned int i=0;
  while(tx_data[i] !='\0')                 // Increment through array, look for null pointer (0) at end of string
  {
    while ((UCA0STATW & UCBUSY));          // Wait if line TX/RX module is busy with data
    UCA0TXBUF = tx_data[i];                // Send out element i of tx_data array on UART bus
    i++;                                   // Increment variable for array address
  }
	// while ((UCA0STATW & UCBUSY));            // Wait if line TX/RX module is busy with data
	// UCA0TXBUF = '\r';
	// while ((UCA0STATW & UCBUSY));            // Wait if line TX/RX module is busy with data
	// UCA0TXBUF = '\n';
}


/****************************************************************
*FUNCTION NAME: printNum()
*FUNCTION     : Allows user to print number of any type to screen
*INPUT        : Number to be printed
*OUTPUT       : none
****************************************************************/
void printNum(unsigned long num)
{
  char tx_data[255];
	sprintf(tx_data, "%lu", num);                   // Cast the number to string
  unsigned int i=0;
  while(tx_data[i] !='\0')                        // Increment through array, look for null pointer (0) at end of string
  {
    while ((UCA0STATW & UCBUSY));                 // Wait if line TX/RX module is busy with data
    UCA0TXBUF = tx_data[i];                       // Send out element i of tx_data array on UART bus
    i++;                                          // Increment variable for array address
  }
	// while ((UCA0STATW & UCBUSY));                   // Wait if line TX/RX module is busy with data
	// UCA0TXBUF = '\r';
	// while ((UCA0STATW & UCBUSY));                   // Wait if line TX/RX module is busy with data
	// UCA0TXBUF = '\n';
}
