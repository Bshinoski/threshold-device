#ifndef LCD_H
#define LCD_H

#include <msp430.h>

/* -------- LCD pin map --------
 * RS → P4.4  (BIT4)
 * E  → P5.3  (BIT3)
 * D4 → P2.5  (BIT5)
 * D5 → P4.3  (BIT3)
 * D6 → P4.2  (BIT2)
 * D7 → P4.1  (BIT1)
 */

#define LCD_RS      BIT4   // P4.4
#define LCD_EN      BIT3   // P5.3
#define LCD_D4      BIT5   // P2.5
#define LCD_D5      BIT3   // P4.3
#define LCD_D6      BIT2   // P4.2
#define LCD_D7      BIT1   // P4.1

#define LCD_RS_PORT P4OUT
#define LCD_EN_PORT P5OUT
#define LCD_D4_PORT P2OUT
#define LCD_D5_PORT P4OUT
#define LCD_D6_PORT P4OUT
#define LCD_D7_PORT P4OUT

#define LCD_RS_DIR  P4DIR
#define LCD_EN_DIR  P5DIR
#define LCD_D4_DIR  P2DIR
#define LCD_D5_DIR  P4DIR
#define LCD_D6_DIR  P4DIR
#define LCD_D7_DIR  P4DIR

/* -------- API -------- */
void LCD_init(void);
void LCD_command(unsigned char cmd);
void LCD_data(unsigned char data);
void LCD_write4bits(unsigned char value);
void LCD_setCursor(unsigned char col, unsigned char row);
void LCD_clear(void);
void LCD_print(const char *string);
void displayTimeOnLCD(unsigned int seconds);   // “MM:SS” helper

#endif /* LCD_H */

