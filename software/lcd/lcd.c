#include "msp430.h"
#include "lcd.h"
#include "../timer/timer.h"
#include "../../tests/mock_delay_cycles.h"

void LCD_init(void) {
    LCD_RS_DIR |= LCD_RS;
    LCD_EN_DIR |= LCD_EN;
    LCD_D4_DIR |= LCD_D4;
    LCD_D5_DIR |= LCD_D5;
    LCD_D6_DIR |= LCD_D6;
    LCD_D7_DIR |= LCD_D7;

    LCD_RS_PORT &= ~LCD_RS;
    LCD_EN_PORT &= ~LCD_EN;
    LCD_D4_PORT &= ~LCD_D4;
    LCD_D5_PORT &= ~LCD_D5;
    LCD_D6_PORT &= ~LCD_D6;
    LCD_D7_PORT &= ~LCD_D7;

    delayMs(50);

    // Force 4-bit mode
    LCD_write4bits(0x03); delayMs(5);
    LCD_write4bits(0x03); delayMs(5);
    LCD_write4bits(0x03); delayMs(1);
    LCD_write4bits(0x02);

    LCD_command(0x28);  // 4-bit, 2-line, 5x8 font
    LCD_command(0x0C);  // Display on, cursor off
    LCD_command(0x01);  // Clear
    delayMs(2);
    LCD_command(0x06);  // Entry mode
}

void LCD_command(unsigned char cmd) {
    LCD_RS_PORT &= ~LCD_RS;
    LCD_write4bits(cmd >> 4);
    LCD_write4bits(cmd & 0x0F);
    if (cmd == 0x01 || cmd == 0x02) delayMs(2);
    else delayMs(1);
}

void LCD_data(unsigned char data) {
    LCD_RS_PORT |= LCD_RS;
    LCD_write4bits(data >> 4);
    LCD_write4bits(data & 0x0F);
    delayMs(1);
}

#ifndef HOST_BUILD
void LCD_write4bits(unsigned char value) {
    if(value & 0x01) LCD_D4_PORT |= LCD_D4; else LCD_D4_PORT &= ~LCD_D4;
    if(value & 0x02) LCD_D5_PORT |= LCD_D5; else LCD_D5_PORT &= ~LCD_D5;
    if(value & 0x04) LCD_D6_PORT |= LCD_D6; else LCD_D6_PORT &= ~LCD_D6;
    if(value & 0x08) LCD_D7_PORT |= LCD_D7; else LCD_D7_PORT &= ~LCD_D7;

    LCD_EN_PORT |= LCD_EN;
    __delay_cycles(10);
    LCD_EN_PORT &= ~LCD_EN;
}
#endif

void LCD_setCursor(unsigned char col, unsigned char row) {
    unsigned char address = (row == 0) ? (0x80 + col) : (0xC0 + col);
    LCD_command(address);
}

void LCD_clear(void) {
    LCD_command(0x01);
    delayMs(2);
}

void LCD_print(const char *string) {
    while(*string) {
        LCD_data(*string++);
    }
}

//------------------------------------------------------
// Show "MM:SS" exactly like your working code
//------------------------------------------------------
void displayTimeOnLCD(unsigned int seconds)
{
    unsigned int mins = seconds / 60;
    unsigned int secs = seconds % 60;
    LCD_setCursor(0, 1);
    if (mins < 10) {
        LCD_data('0');
    }
    LCD_data('0' + (mins % 10));
    LCD_data(':');
    LCD_data('0' + (secs / 10));  // Tens digit of seconds
    LCD_data('0' + (secs % 10));  // Units digit of seconds
}
