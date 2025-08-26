#ifndef SERIALMONITOR_h
#define SERIALMONITOR_h

#include <msp430.h>
#include <string.h>
#include <stdio.h>

void configureClockAndUART();               // Clock set to 8MHz and Baud rate at 9600
void printStr(const char *tx_data);         // Print string to the screen
void printNum(unsigned long num);           // Print number to the screen

#endif
