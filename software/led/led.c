#include "led.h"

void initLEDs(void)
{
    GREEN_LED_DIR |= GREEN_LED_PIN;   // green as output
    RED_LED_DIR   |= RED_LED_PIN;     // red   as output

    GREEN_LED_OUT |= GREEN_LED_PIN;   // green ON
    RED_LED_OUT   &= ~RED_LED_PIN;    // red OFF
}

void setGreenLED(uint8_t on)
{
    if (on)  GREEN_LED_OUT |=  GREEN_LED_PIN;
    else     GREEN_LED_OUT &= ~GREEN_LED_PIN;
}

void setRedLED(uint8_t on)
{
    if (on)  RED_LED_OUT |=  RED_LED_PIN;
    else     RED_LED_OUT &= ~RED_LED_PIN;
}

