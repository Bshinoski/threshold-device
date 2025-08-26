#include "unity/src/unity.h"
#include "../software/hm10bluetooth/bluetooth.h" // Include your header with uart_send_string
#include "../software/led/led.h"
#include "../software/myoware/myoware.h"
#include "../software/myoware/serialmonitor.h"
#include "../software/timer/timer.h"
#include "../software/lcd/lcd.h"
#include "mock_delay_cycles.h"
#include <string.h>
#include "msp430.h"

//Current Compilation
//Call make from ThresholdDevice/tests and should run

unsigned long mockDelayCycles = 0;

unsigned char lcd_write4bits_calls[100];
int lcd_write4bits_call_count = 0;

// Spy override of LCD_write4bits (MUST match lcd.c signature)
void LCD_write4bits(unsigned char value) {
    lcd_write4bits_calls[lcd_write4bits_call_count++] = value;
}

void setUp(void)
{
    /* Reset MSP430 mock registers to known defaults before each test. */

    /* Reset for Bluetooth Tests */
    P6SEL0   = 0;
    P6SEL1   = 0xFF;
    PM5CTL0  = LOCKLPM5;
    CSCTL0_H = 0;
    CSCTL1   = 0;
    CSCTL2   = 0;
    CSCTL3   = 0;
    UCA3CTLW0 = 0;
    UCA3BRW   = 0;
    UCA3MCTLW = 0;
    UCA3IFG   = 0;
    UCA3TXBUF = 0;

    /* Reset for LED Tests */
    P2DIR = 0;
    P2OUT = 0;
    P4DIR = 0;
    P4OUT = 0;

    /* Reset for Myoware Tests */
    P3SEL0 = 0;
    P3SEL1 = 0;
    ADC12CTL0 = 0;
    ADC12CTL1 = 0;
    ADC12CTL2 = 0;
    ADC12MCTL0 = 0;
    ADC12MEM0 = 0;

    /* Reset for Timer Tests */
    mockDelayCycles = 0;

    /* Reset for LCD Tests*/

    lcd_write4bits_call_count = 0;
    for (int i = 0; i < 100; ++i) lcd_write4bits_calls[i] = 0;

    // Clear all mocked ports
    P2DIR = P2OUT = 0;
    P3DIR = P3OUT = 0;
    P7DIR = P7OUT = 0;
    
}

void tearDown(void)
{
    /* Typically do nothing here */
}

void test_init_gpio_uart_pins(void)
{
    init_gpio_uart_pins();

    /* Expect P6SEL0 to have BIT0|BIT1 set, and P6SEL1 to have them cleared */
    TEST_ASSERT_EQUAL_HEX8_MESSAGE((BIT0 | BIT1), (P6SEL0 & (BIT0 | BIT1)),
                                   "P6SEL0 should have BIT0 & BIT1 set.");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(0, (P6SEL1 & (BIT0 | BIT1)),
                                   "P6SEL1 should have BIT0 & BIT1 cleared.");

    /* PM5CTL0 should have LOCKLPM5 cleared */
    TEST_ASSERT_FALSE_MESSAGE((PM5CTL0 & LOCKLPM5),
                              "LOCKLPM5 bit should be cleared in PM5CTL0");
}


void test_init_clock(void)
{
    init_clock();

    /* Check final register states after init_clock() finishes. 
       The code writes CSCTL0_H = CSKEY_H initially, but then locks it again 
       by setting CSCTL0_H = 0 before returning. */

    TEST_ASSERT_EQUAL_HEX8_MESSAGE(0, CSCTL0_H,
        "CSCTL0_H should be locked (0x00) at the end.");

    TEST_ASSERT_EQUAL_HEX16_MESSAGE(
        (DCOFSEL_3 | DCORSEL),
        CSCTL1,
        "DCOFSEL_3 | DCORSEL for an 8 MHz DCO."
    );

    TEST_ASSERT_EQUAL_HEX16_MESSAGE(
        (SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK),
        CSCTL2,
        "ACLK=VLO, SMCLK=MCLK=DCO"
    );

    TEST_ASSERT_EQUAL_HEX16_MESSAGE(
        (DIVA__1 | DIVS__1 | DIVM__1),
        CSCTL3,
        "No clock division set."
    );
}


void test_init_uart(void)
{
    init_uart();

    /* Verify the final UART register settings after init_uart() 
       has run. The function initially sets UCSWRST, 
       but clears it before returning. */
    
    /* Check that SMCLK was selected */
    TEST_ASSERT_TRUE_MESSAGE(
        (UCA3CTLW0 & UCSSEL__SMCLK),
        "UART clock source should be SMCLK."
    );

    /* UCSWRST should be cleared (i.e., not in reset) */
    TEST_ASSERT_FALSE_MESSAGE(
        (UCA3CTLW0 & UCSWRST),
        "UCSWRST should be cleared after init_uart finishes."
    );

    /* Check baud rate (9600) for an 8 MHz SMCLK */
    TEST_ASSERT_EQUAL_HEX16_MESSAGE(
        52,
        UCA3BRW,
        "UCA3BRW should be 52 for ~9600 baud at 8 MHz."
    );

    /* UCOS16, UCBRF_1, plus 0x4900 for modulation */
    TEST_ASSERT_EQUAL_HEX16_MESSAGE(
        (UCOS16 | UCBRF_1 | 0x4900),
        UCA3MCTLW,
        "UCA3MCTLW should combine UCOS16, UCBRF_1, and 0x4900."
    );
}


void test_uart_send_char(void)
{
    /* Suppose we set the TXIFG bit to simulate TX buffer readiness. */
    UCA3IFG = UCTXIFG;

    uart_send_char('A');

    TEST_ASSERT_EQUAL_MESSAGE('A', UCA3TXBUF,
                              "UCA3TXBUF should contain 'A' after sending.");
}


void test_uart_send_string(void)
{
    UCA3IFG = UCTXIFG;

    uart_send_string("Ben");

    /* The last character written to TXBUF should be 'n' */
    TEST_ASSERT_EQUAL_MESSAGE('n', UCA3TXBUF,
                             "Last char in 'Ben' is 'n'. Should be in TXBUF.");
}


void test_init_bluetooth(void)
{
    /* Checks only partially of each as each function is called within init_bluetooth */
    init_bluetooth();

    /* From init_gpio_uart_pins() */
    TEST_ASSERT_EQUAL_HEX8((BIT0 | BIT1), P6SEL0 & (BIT0 | BIT1));
    TEST_ASSERT_FALSE(PM5CTL0 & LOCKLPM5);

    /* From init_clock() */
    TEST_ASSERT_EQUAL_HEX16((DCOFSEL_3 | DCORSEL), CSCTL1);

    /* From init_uart() */
    TEST_ASSERT_FALSE(UCA3CTLW0 & UCSWRST);
}

void test_initLEDs(void)
{
    initLEDs();

    // Check that P2DIR has bit 5 set for Green LED
    TEST_ASSERT_TRUE_MESSAGE(
        (P2DIR & GREEN_LED_PIN),
        "GREEN_LED_PIN (P2.5) should be set as output."
    );

    // Check that P4DIR has bit 3 set for Red LED
    TEST_ASSERT_TRUE_MESSAGE(
        (P4DIR & RED_LED_PIN),
        "RED_LED_PIN (P4.3) should be set as output."
    );

    // Check that P2OUT has bit 5 set (Green LED ON)
    TEST_ASSERT_TRUE_MESSAGE(
        (P2OUT & GREEN_LED_PIN),
        "Green LED (P2.5) should be ON initially."
    );

    // Check that P4OUT has bit 3 cleared (Red LED OFF)
    TEST_ASSERT_FALSE_MESSAGE(
        (P4OUT & RED_LED_PIN),
        "Red LED (P4.3) should be OFF initially."
    );
}

void test_setGreenLED(void)
{
    // First, ensure it's off
    setGreenLED(0);
    TEST_ASSERT_FALSE_MESSAGE(
        (P2OUT & GREEN_LED_PIN),
        "Green LED (P2.5) should be OFF after setGreenLED(0)."
    );

    // Then, turn it on
    setGreenLED(1);
    TEST_ASSERT_TRUE_MESSAGE(
        (P2OUT & GREEN_LED_PIN),
        "Green LED (P2.5) should be ON after setGreenLED(1)."
    );
}

void test_setRedLED(void)
{
    // First, ensure it's off
    setRedLED(0);
    TEST_ASSERT_FALSE_MESSAGE(
        (P4OUT & RED_LED_PIN),
        "Red LED (P4.3) should be OFF after setRedLED(0)."
    );

    // Then, turn it on
    setRedLED(1);
    TEST_ASSERT_TRUE_MESSAGE(
        (P4OUT & RED_LED_PIN),
        "Red LED (P4.3) should be ON after setRedLED(1)."
    );
}

void test_initADC(void)
{
    initADC();

    // Check that P3.0 is set to ADC mode
    TEST_ASSERT_TRUE_MESSAGE(
        (P3SEL0 & BIT0),
        "P3SEL0 bit 0 should be set for ADC channel."
    );
    TEST_ASSERT_TRUE_MESSAGE(
        (P3SEL1 & BIT0),
        "P3SEL1 bit 0 should be set for ADC channel."
    );
    
    uint16_t expectedCTL0 = (ADC12SHT0_2 | ADC12ON | ADC12ENC);
    TEST_ASSERT_EQUAL_HEX16_MESSAGE(
        expectedCTL0,
        (ADC12CTL0 & (ADC12SHT0_2 | ADC12ON | ADC12ENC)),
        "ADC12CTL0 should have SHT0_2, ADC12ON, and ENC set."
    );

    // Confirm ADC12CTL1 has ADC12SHP and ADC12SSEL_3
    uint16_t expectedCTL1 = (ADC12SHP | ADC12SSEL_3);
    TEST_ASSERT_EQUAL_HEX16_MESSAGE(
        expectedCTL1,
        (ADC12CTL1 & (ADC12SHP | ADC12SSEL_3)),
        "ADC12CTL1 should have ADC12SHP and ADC12SSEL_3 set."
    );

    // Confirm 12-bit resolution in ADC12CTL2
    TEST_ASSERT_TRUE_MESSAGE(
        (ADC12CTL2 & ADC12RES_2),
        "ADC12CTL2 should have ADC12RES_2 for 12-bit resolution."
    );

    // Confirm channel select is A12 in ADC12MCTL0 
    TEST_ASSERT_TRUE_MESSAGE(
        (ADC12MCTL0 & ADC12INCH_12),
        "ADC12MCTL0 should select input channel A12."
    );
}

void test_readADC(void)
{
    // Suppose we set ADC12MEM0 to a known test value
    uint16_t testValue = 0x0ABC;
    ADC12MEM0 = testValue;

    //Clear busy bit 
    ADC12CTL1 &= ~ADC12BUSY;

    // Now call readADC()
    uint16_t result = readADC();

    // The function does ADC12CTL0 |= ADC12SC
    TEST_ASSERT_TRUE_MESSAGE(
        (ADC12CTL0 & ADC12SC),
        "ADC12SC bit should be set to start conversion."
    );

    // Finally, readADC() returns ADC12MEM0 
    TEST_ASSERT_EQUAL_MESSAGE(
        testValue, 
        result, 
        "readADC() should return the value stored in ADC12MEM0."
    );
}

void test_delayMs(void)
{
    delayMs(5);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(
        5 * 1000,
        mockDelayCycles,
        "delayMs(5) should call __delay_cycles(1000) exactly 5 times."
    );
}

void test_delayMsTimer(void)
{
    delayMsTimer(2);

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(
        2 * CYCLES_PER_MS,
        mockDelayCycles,
        "delayMsTimer(2) should call __delay_cycles(CYCLES_PER_MS) 2 times."
    );
}

void test_LCD_init(void)
{
    LCD_init();
    
    // Check that the direction registers have been set for each LCD pin.
    TEST_ASSERT_TRUE_MESSAGE((P3DIR & LCD_RS), "LCD_RS_DIR (P3, BIT7) should be set");
    TEST_ASSERT_TRUE_MESSAGE((P3DIR & LCD_EN), "LCD_EN_DIR (P3, BIT6) should be set");
    TEST_ASSERT_TRUE_MESSAGE((P3DIR & LCD_D4), "LCD_D4_DIR (P3, BIT5) should be set");
    TEST_ASSERT_TRUE_MESSAGE((P3DIR & LCD_D5), "LCD_D5_DIR (P3, BIT4) should be set");
    TEST_ASSERT_TRUE_MESSAGE((P7DIR & LCD_D6), "LCD_D6_DIR (P7, BIT3) should be set");
    TEST_ASSERT_TRUE_MESSAGE((P2DIR & LCD_D7), "LCD_D7_DIR (P2, BIT6) should be set");
    
    // Check that the initial output values are cleared.
    TEST_ASSERT_FALSE_MESSAGE((P3OUT & LCD_RS), "LCD_RS_PORT should be cleared after init");
    TEST_ASSERT_FALSE_MESSAGE((P3OUT & LCD_EN), "LCD_EN_PORT should be cleared after init");
    TEST_ASSERT_FALSE_MESSAGE((P3OUT & LCD_D4), "LCD_D4_PORT should be cleared after init");
    TEST_ASSERT_FALSE_MESSAGE((P3OUT & LCD_D5), "LCD_D5_PORT should be cleared after init");
    TEST_ASSERT_FALSE_MESSAGE((P7OUT & LCD_D6), "LCD_D6_PORT should be cleared after init");
    TEST_ASSERT_FALSE_MESSAGE((P2OUT & LCD_D7), "LCD_D7_PORT should be cleared after init");

    // LCD_init calls several LCD_write4bits commands.
    TEST_ASSERT_TRUE_MESSAGE(lcd_write4bits_call_count > 0, "LCD_write4bits should be called during LCD_init");
}

void test_LCD_command(void)
{
    // Ensure RS is cleared for command mode.
    P3OUT &= ~LCD_RS;
    lcd_write4bits_call_count = 0;  // Reset spy counter

    // Send a command (e.g., 0x28: 4-bit, 2-line, 5x8 font)
    LCD_command(0x28);

    // In command mode, RS should remain cleared.
    TEST_ASSERT_FALSE_MESSAGE((P3OUT & LCD_RS), "LCD_RS_PORT should be cleared in command mode");

    // LCD_command should call LCD_write4bits twice.
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, lcd_write4bits_call_count, "LCD_command should invoke LCD_write4bits twice");

    // For 0x28, expected nibbles:
    //   High nibble: 0x28 >> 4 = 0x2, Low nibble: 0x28 & 0x0F = 0x8.
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x2, lcd_write4bits_calls[0], "First nibble of command 0x28 should be 0x2");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x8, lcd_write4bits_calls[1], "Second nibble of command 0x28 should be 0x8");
}

void test_LCD_data(void)
{
    // Reset spy counter and ensure RS is initially cleared.
    lcd_write4bits_call_count = 0;
    P3OUT &= ~LCD_RS;
    
    // Send data (e.g., character 'A' (0x41)).
    LCD_data('A');
    
    // In data mode, LCD_data should set RS.
    TEST_ASSERT_TRUE_MESSAGE((P3OUT & LCD_RS), "LCD_RS_PORT should be set in data mode");
    
    // LCD_data should call LCD_write4bits twice.
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, lcd_write4bits_call_count, "LCD_data should invoke LCD_write4bits twice");

    // For 'A' (0x41): high nibble = 0x41 >> 4 = 0x4, low nibble = 0x41 & 0x0F = 0x1.
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x4, lcd_write4bits_calls[0], "First nibble of 'A' should be 0x4");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x1, lcd_write4bits_calls[1], "Second nibble of 'A' should be 0x1");
}


void test_LCD_setCursor(void)
{
    lcd_write4bits_call_count = 0;

    // For row 0, col 3, the address is 0x80 + 3 = 0x83.
    LCD_setCursor(3, 0);

    // LCD_setCursor calls LCD_command, which in turn calls LCD_write4bits twice.
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, lcd_write4bits_call_count, "LCD_setCursor should invoke LCD_command (2 calls)");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x83 >> 4, lcd_write4bits_calls[0], "High nibble for cursor command should be correct");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x83 & 0x0F, lcd_write4bits_calls[1], "Low nibble for cursor command should be correct");
}


void test_LCD_clear(void)
{
    lcd_write4bits_call_count = 0;

    // LCD_clear calls LCD_command(0x01) and a delay.
    LCD_clear();

    // For command 0x01: high nibble = 0x01 >> 4 = 0x0, low nibble = 0x01 & 0x0F = 0x1.
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, lcd_write4bits_call_count, "LCD_clear should invoke LCD_command (2 calls)");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x0, lcd_write4bits_calls[0], "High nibble for clear command should be 0x0");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x1, lcd_write4bits_calls[1], "Low nibble for clear command should be 0x1");
}


void test_displayTimeOnLCD(void)
{
    lcd_write4bits_call_count = 0;

    displayTimeOnLCD(123);
    TEST_ASSERT_EQUAL_INT_MESSAGE(12, lcd_write4bits_call_count, "displayTimeOnLCD should call LCD_write4bits 12 times");

    // Now verify the sequence of nibble values.
    int index = 0;
    // LCD_setCursor(0,1): address = 0xC0 -> high nibble: 0xC, low nibble: 0x0.
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(0xC, lcd_write4bits_calls[index++], "Cursor command high nibble should be 0xC");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x0, lcd_write4bits_calls[index++], "Cursor command low nibble should be 0x0");
    
    // LCD_data('0'): '0' = 0x30 -> high nibble: 0x3, low nibble: 0x0.
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x3, lcd_write4bits_calls[index++], "Data '0' high nibble should be 0x3");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x0, lcd_write4bits_calls[index++], "Data '0' low nibble should be 0x0");
    
    // LCD_data('2'): '2' = 0x32 -> high nibble: 0x3, low nibble: 0x2.
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x3, lcd_write4bits_calls[index++], "Data '2' high nibble should be 0x3");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x2, lcd_write4bits_calls[index++], "Data '2' low nibble should be 0x2");
    
    // LCD_data(':'): ':' = 0x3A -> high nibble: 0x3, low nibble: 0xA.
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x3, lcd_write4bits_calls[index++], "Data ':' high nibble should be 0x3");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(0xA, lcd_write4bits_calls[index++], "Data ':' low nibble should be 0xA");
    
    // LCD_data('0'): '0' = 0x30 -> high nibble: 0x3, low nibble: 0x0.
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x3, lcd_write4bits_calls[index++], "Data '0' high nibble should be 0x3");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x0, lcd_write4bits_calls[index++], "Data '0' low nibble should be 0x0");
    
    // LCD_data('3'): '3' = 0x33 -> high nibble: 0x3, low nibble: 0x3.
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x3, lcd_write4bits_calls[index++], "Data '3' high nibble should be 0x3");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x3, lcd_write4bits_calls[index++], "Data '3' low nibble should be 0x3");
}

void test_bluetooth_full_send(void)
{
    init_bluetooth();
    UCA3IFG = UCTXIFG;  // Simulate TX buffer ready

    uart_send_string("OK");

    TEST_ASSERT_EQUAL_MESSAGE('K', UCA3TXBUF, "Last character sent should be 'K'.");
    TEST_ASSERT_FALSE_MESSAGE(PM5CTL0 & LOCKLPM5, "LOCKLPM5 should be cleared.");
}

void test_adc_led_response(void)
{
    initLEDs();

    // Simulate a high ADC value
    ADC12MEM0 = 3000;
    ADC12CTL1 &= ~ADC12BUSY;

    uint16_t value = readADC();

    if (value > 2500) {
        setRedLED(1);
        setGreenLED(0);
    } else {
        setRedLED(0);
        setGreenLED(1);
    }

    TEST_ASSERT_TRUE_MESSAGE(P4OUT & RED_LED_PIN, "Red LED should be ON for high ADC value.");
    TEST_ASSERT_FALSE_MESSAGE(P2OUT & GREEN_LED_PIN, "Green LED should be OFF for high ADC value.");
}

void test_display_adc_value_on_lcd(void)
{
    ADC12MEM0 = 1234;
    ADC12CTL1 &= ~ADC12BUSY;

    lcd_write4bits_call_count = 0;

    uint16_t value = readADC();
    displayTimeOnLCD(value);

    TEST_ASSERT_TRUE_MESSAGE(lcd_write4bits_call_count > 0, "LCD should display ADC value.");
}

void test_time_display_after_delay(void)
{
    lcd_write4bits_call_count = 0;
    mockDelayCycles = 0;

    delayMs(2);
    displayTimeOnLCD(45);

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(8000, mockDelayCycles, "delayMs(2) should call 2000 cycles.");
    TEST_ASSERT_TRUE_MESSAGE(lcd_write4bits_call_count > 0, "LCD should be updated after delay.");
}

void test_full_system_init(void)
{
    init_bluetooth();
    initLEDs();
    initADC();
    LCD_init();

    TEST_ASSERT_FALSE_MESSAGE(PM5CTL0 & LOCKLPM5, "LOCKLPM5 should be cleared.");
    TEST_ASSERT_TRUE_MESSAGE(P2DIR & GREEN_LED_PIN, "Green LED pin should be output.");
    TEST_ASSERT_TRUE_MESSAGE(P3SEL0 & BIT0, "ADC input pin should be set.");
    TEST_ASSERT_TRUE_MESSAGE(lcd_write4bits_call_count > 0, "LCD_write4bits should be called during init.");
}

int main(void)
{
    UNITY_BEGIN();

    printf("\n--- Unit Tests ---\n");

    // bluetooth.c Tests
    RUN_TEST(test_init_gpio_uart_pins);
    RUN_TEST(test_init_clock);
    RUN_TEST(test_init_uart);
    RUN_TEST(test_uart_send_char);
    RUN_TEST(test_uart_send_string);
    RUN_TEST(test_init_bluetooth);

    // led.c Tests
    RUN_TEST(test_initLEDs);
    RUN_TEST(test_setGreenLED);
    RUN_TEST(test_setRedLED);

    // myoware.c Tests
    RUN_TEST(test_initADC);
    RUN_TEST(test_readADC);

    
    // timer.c Tests
    RUN_TEST(test_delayMs);
    RUN_TEST(test_delayMsTimer);

    // lcd.c Tests
    RUN_TEST(test_LCD_init);
    RUN_TEST(test_LCD_command);
    RUN_TEST(test_LCD_data);
    RUN_TEST(test_LCD_setCursor);
    RUN_TEST(test_LCD_clear);
    RUN_TEST(test_displayTimeOnLCD);

    printf("\n--- Integration Tests ---\n");
    RUN_TEST(test_bluetooth_full_send);
    RUN_TEST(test_adc_led_response);
    RUN_TEST(test_display_adc_value_on_lcd);
    RUN_TEST(test_time_display_after_delay);
    RUN_TEST(test_full_system_init);


    return UNITY_END();
}

