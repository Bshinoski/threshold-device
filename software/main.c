#include <msp430.h>
#include "hm10bluetooth/bluetooth.h"
#include <stdint.h>
#include "myoware/serialmonitor.h"
#include "myoware/myoware.h"
#include "lcd/lcd.h"
#include "timer/timer.h"
#include "led/led.h"

// ------------------ Button Definitions ------------------
#define SELECT_BUTTON   BIT5  // P5.5
#define START_BUTTON    BIT6  // P5.6

// ------------------ Timer Durations ------------------
static const unsigned int timerDurations[] = {60, 90, 120, 150, 180}; // seconds
#define NUM_TIMERS (sizeof(timerDurations)/sizeof(timerDurations[0]))
// (Optional) user-friendly labels if desired:
// const char* timerLabels[] = {"1:00","1:30","2:00","2:30","3:00"};

// ------------------ EMG/Failure Constants (unchanged) ------------------
#define THRESH_START   200
#define THRESH_END     100
#define FAILURE_RATIO  0.6f
#define MIN_REPS_BEFORE_FAILURE 3
#define MIN_REP_DURATION 50
#define MAX_FAILURE_DURATION 50

// ------------------ Globals ------------------
static unsigned char currentTimerIndex = 0;

// Timer State
static unsigned char countdownActive = 0; 
static unsigned long countdownStartTime = 0;
static unsigned long currentTime = 0;     
static unsigned char timerCompleted = 0;

// EMG/Rep Detection
static unsigned char record = 0;
static unsigned char inRep = 0;
static uint16_t currentPeak = 0;
static unsigned int repDuration = 0;

static uint16_t highestPeak = 0;
static unsigned int repCount = 0;
static unsigned char failureDetected = 0;

static unsigned char shortRepCount = 0;
static unsigned int lastValidRepTime = 0;
static unsigned int emgTimeCounter = 0;

// ------------------ Main ------------------
int main(void)
{
    //--- Same MyoWare logic & sampling as your provided file ---
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5; 
    configureClockAndUART();
    initADC();
    init_bluetooth();

    // --- Configure button P5.6 (toggle recording) + P5.5 (select) ---
    P5DIR &= ~(START_BUTTON | SELECT_BUTTON);
    P5REN |= (START_BUTTON | SELECT_BUTTON);
    P5OUT |= (START_BUTTON | SELECT_BUTTON);
    unsigned char lastButtonState = (P5IN & START_BUTTON) ? 1 : 0;
    unsigned char lastSelectState = (P5IN & SELECT_BUTTON) ? 1 : 0;

    // Set initial state of LEDs
    initLEDs();

    // --- LCD init ---
    LCD_init();
    LCD_clear();
    delayMs(10);  // Extra delay to ensure clear completes
    // Show "Timer Selection:" at top, then show initial time at row 1
    LCD_setCursor(0,0);
    LCD_print("Timer Selection:");
    LCD_setCursor(0,1);
    // If you prefer full label, manually print e.g. "01:00" or see optional 'timerLabels'
    // For now, just show numeric:
    displayTimeOnLCD(timerDurations[currentTimerIndex]);

    while (1)
    {
        // ---------- SELECT Button logic (P5.5) ----------
        unsigned char currentSelectState = (P5IN & SELECT_BUTTON) ? 1 : 0;
        if (lastSelectState == 1 && currentSelectState == 0) {
            // Pressed
            if(!countdownActive && !failureDetected) {
                // Cycle to next time
                currentTimerIndex = (currentTimerIndex + 1) % NUM_TIMERS;

                // Update LCD
                LCD_clear();
                LCD_setCursor(0,0);
                LCD_print("Timer Selection:");
                LCD_setCursor(0,1);
                displayTimeOnLCD(timerDurations[currentTimerIndex]);
            }
        }
        lastSelectState = currentSelectState;

        // ---------- START Button logic (P5.6) for MyoWare record ----------
        unsigned char currentBtnState = (P5IN & START_BUTTON) ? 1 : 0;
        if (lastButtonState == 1 && currentBtnState == 0) {
            // Pressed
            if(!record && !failureDetected && !countdownActive) {
                record = 1;
                emgTimeCounter = 0;
                inRep = 0;
                repCount = 0;
                failureDetected = 0;
                shortRepCount = 0;
                highestPeak = 0;
                lastValidRepTime = 0;
                printStr("Recording EMG started.\r\n");
            }
        }
        lastButtonState = currentBtnState;

        // ---------- EMG logic (unchanged) ----------
        if (record && !failureDetected) {
            uint16_t emg_signal = readADC();
            printNum(emg_signal);
            printStr("\r\n");

            if (!inRep) {
                if (emg_signal > THRESH_START) {
                    inRep = 1;
                    currentPeak = emg_signal;
                    repDuration = 0;
                }
            }
            else {
                repDuration++;
                if (emg_signal > currentPeak) {
                    currentPeak = emg_signal;
                }
                if (emg_signal < THRESH_END) {
                    inRep = 0;
                    if (repDuration >= MIN_REP_DURATION) {
                        repCount++;
                        if (currentPeak > highestPeak) {
                            highestPeak = currentPeak;
                        }
                        lastValidRepTime = emgTimeCounter;
                        shortRepCount = 0;

                        printStr("Rep ");
                        printNum(repCount);
                        printStr(" peak: ");
                        printNum(currentPeak);
                        printStr(" highest: ");
                        printNum(highestPeak);
                        printStr("\r\n");

                        if (repCount >= MIN_REPS_BEFORE_FAILURE && highestPeak > 0) {
                            float peakRatio = (float)currentPeak / highestPeak;
                            if (peakRatio < FAILURE_RATIO) {
                                failureDetected = 1;
                                record = 0;
                                
                                setGreenLED(0);
                                setRedLED(1);

                                printStr("FAILURE DETECTED! Peak ratio: ");
                                printNum((uint16_t)(peakRatio * 100));
                                printStr("%\r\n");

                                // Timer starts
                                countdownActive = 1;
                                timerCompleted  = 0;
                                countdownStartTime = currentTime;
                                printStr("Countdown timer started.\r\n");
                            }
                        }
                    }
                    else if (repDuration <= MAX_FAILURE_DURATION) {
                        shortRepCount++;
                        if (repCount >= MIN_REPS_BEFORE_FAILURE && shortRepCount >= 3 && 
                            (emgTimeCounter - lastValidRepTime) < 1000) {

                            failureDetected = 1;
                            record = 0;
                            setGreenLED(0);
                            setRedLED(1);

                            printStr("FAILURE DETECTED! Multiple short reps\r\n");

                            // Timer starts
                            countdownActive = 1;
                            timerCompleted  = 0;
                            countdownStartTime = currentTime;
                            printStr("Countdown timer started.\r\n");
                        }
                    }
                }
            }
            emgTimeCounter++;

            // ***Sampling delay is unchanged from your file***
            __delay_cycles(50000);  // ~50ms if ~1MHz clock
        }

        // ---------- Countdown Timer if active ----------
        if(countdownActive && !timerCompleted) {
            // As in your prior merges, ~1s steps:
            delayMsTimer(1000); 
            currentTime++;

            unsigned long elapsed = currentTime - countdownStartTime;
            unsigned int totalSec = timerDurations[currentTimerIndex];

            if(elapsed >= totalSec) {
                timerCompleted = 1;
                LCD_clear();
                delayMs(10);  // Ensure clear completes
		        LCD_setCursor(0,0);
                LCD_print("Time's up!");
                LCD_setCursor(0,1);
                displayTimeOnLCD(0);

                printStr("TIMER COMPLETE.\r\n");
            } else {
                unsigned int remain = totalSec - (unsigned int)elapsed;
                LCD_clear();
		        delayMs(10);  // Ensure clear completes
                LCD_setCursor(0,0);
                LCD_print("Time Remaining:");
                LCD_setCursor(0,1);
                displayTimeOnLCD(remain);
            }
        }
        else {
            delayMs(100); // Tiny idle delay
        }
    }
}
