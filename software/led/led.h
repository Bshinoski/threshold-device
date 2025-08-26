#ifndef LED_H
#define LED_H

#include <stdint.h>
#include <msp430.h>

/* -------- LED pin definitions -------- */
/* Green → P6.2 ,  Red → P4.7 */
#define GREEN_LED_PIN   BIT2          // P6.2
#define RED_LED_PIN     BIT7          // P4.7

#define GREEN_LED_DIR   P6DIR
#define GREEN_LED_OUT   P6OUT

#define RED_LED_DIR     P4DIR
#define RED_LED_OUT     P4OUT

/* -------- API -------- */
void initLEDs(void);
void setGreenLED(uint8_t on);
void setRedLED(uint8_t on);

#endif /* LED_H */

