//Global libraries
#include <Arduino.h>
#include "config.h"
#include "button.h"

// Minimal time between two accepted presses for each button
#define LOCK_TIME 150
#define DEBOUNCE_TIME 30

//Store the state of every button
bool rawState[5] = {false, false, false, false, false};
bool stableState[5] = {false, false, false, false, false};
bool justPressedState[5] = {false, false, false, false, false};

//Store the time at which the state changed
unsigned long lastChangeTime[5] = {0, 0, 0, 0, 0};
unsigned long lastPressTime[5] = {0, 0, 0, 0, 0};

//Counters used for setting the time and alarms
volatile uint8_t plusPressCount = 0;
volatile uint8_t minusPressCount = 0;
volatile uint8_t validatePressCount = 0;

//Variables used to record the last update
volatile unsigned long lastPlusInterrupt = 0;
volatile unsigned long lastMinusInterrupt = 0;
volatile unsigned long lastValidateInterrupt = 0;

//Interrupts functions are stored into RAM to allow access even during refresh where the flash is not accessible
//Interrupts are used to detect key presses even whilst the screen is being refreshed
void IRAM_ATTR plus_interrupt()
{
    unsigned long now = micros(); //Interrupts functions works in microseconds

    if (now - lastPlusInterrupt >= DEBOUNCE_TIME * 5000UL)
    {
        if (plusPressCount < 255)
            plusPressCount++;

        lastPlusInterrupt = now;
    }
}

void IRAM_ATTR minus_interrupt()
{
    unsigned long now = micros();

    if (now - lastMinusInterrupt >= DEBOUNCE_TIME * 5000UL)
    {
        if (minusPressCount < 255)
            minusPressCount++;

        lastMinusInterrupt = now;
    }
}

void IRAM_ATTR validate_interrupt()
{
    unsigned long now = micros();

    if (now - lastValidateInterrupt >= DEBOUNCE_TIME * 5000UL)
    {
        if (validatePressCount < 255)
            validatePressCount++;

        lastValidateInterrupt = now;
    }
}

void buttons_init()
{
    //Initialise all buttons with their pull-up resistors and interrupts
    pinMode(SWITCH_LIGHT_PIN, INPUT_PULLUP);
    pinMode(BUTTON_PLUS_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PLUS_PIN), plus_interrupt, FALLING);
    pinMode(BUTTON_MINUS_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_MINUS_PIN), minus_interrupt, FALLING);
    pinMode(BUTTON_VALIDATE_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_VALIDATE_PIN), validate_interrupt, FALLING);
    pinMode(BUTTON_PARAM_PIN, INPUT_PULLUP); 
    
    unsigned long now = millis();

    //For every buttons we initialise the previous tabs
    for (int i = 0; i < 5; i++)
    {
        bool state = digitalRead(getButtonPin((Button)i)) == LOW;

        rawState[i] = state;
        stableState[i] = state;
        lastPressTime[i] = now - getLockTime((Button)i);
    }
}

//Update button state
void buttons_update()
{
    unsigned long now = millis();

    for (int i = 1; i < 5; i++)
    {
        Button button = (Button)i;
        button_clear_press(button);

        bool currentRaw = digitalRead(getButtonPin(button)) == LOW;

        if (currentRaw != rawState[i])
        {
            rawState[i] = currentRaw;
            lastChangeTime[i] = now;
        }

        // Avoid BOUNCING effect
        if ((now - lastChangeTime[i]) >= DEBOUNCE_TIME)
        {
            if (stableState[i] != rawState[i])
            {
                stableState[i] = rawState[i];

                if (stableState[i])                
                {
                    unsigned long lockTime = getLockTime(button);

                    if ((now - lastPressTime[i]) >= lockTime)
                    {
                        justPressedState[i] = true;
                        lastPressTime[i] = now;
                    }
                    else
                    {
                        justPressedState[i] = false;
                    }
                }
            }
        }
    }
}

//Turn the light ON
bool switch_light_state()
{
    return digitalRead(SWITCH_LIGHT_PIN) == HIGH;
}

bool button_pressed(Button button)
{
    return stableState[button];
}

bool button_just_pressed(Button button)
{
    return justPressedState[button];
}

//Avoid problems caused by using the same button for different functions
void button_clear_press(Button button)
{
    justPressedState[button] = false;
}

int getButtonPin(Button button)
{
    switch (button)
    {
        case SWITCH_LIGHT:    return SWITCH_LIGHT_PIN;
        case BUTTON_PLUS:     return BUTTON_PLUS_PIN;
        case BUTTON_MINUS:    return BUTTON_MINUS_PIN;
        case BUTTON_VALIDATE: return BUTTON_VALIDATE_PIN;
        case BUTTON_PARAM:    return BUTTON_PARAM_PIN;
    }
    return -1;
}

//The lock duration is only used for the PLUS and MINUS buttons to prevent multiple clicks in once
unsigned long getLockTime(Button button)
{
    if (button == BUTTON_PLUS || button == BUTTON_MINUS)
        return LOCK_TIME;

    return 0;
}

//Getters with interrupt
uint8_t button_get_plus_count()
{
    noInterrupts();

    uint8_t count = plusPressCount;
    plusPressCount = 0;

    interrupts();

    return count;
}

uint8_t button_get_minus_count()
{
    noInterrupts();

    uint8_t count = minusPressCount;
    minusPressCount = 0;

    interrupts();

    return count;
}

uint8_t button_get_validate_count()
{
    noInterrupts();

    uint8_t count = validatePressCount;
    validatePressCount = 0;

    interrupts();

    return count;
}

//Resets counters
void button_clear_plus_minus_count()
{
    noInterrupts();

    plusPressCount = 0;
    minusPressCount = 0;

    interrupts();
}

void button_clear_validate_count()
{
    noInterrupts();

    validatePressCount = 0;

    interrupts();
}