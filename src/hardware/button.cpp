#include "button.h"

// Minimal time between two accepted presses for each button
#define LOCK_TIME 150

#define DEBOUNCE_TIME 30

bool rawState[5] = {false, false, false, false, false};
bool stableState[5] = {false, false, false, false, false};
bool justPressedState[5] = {false, false, false, false, false};

unsigned long lastChangeTime[5] = {0, 0, 0, 0, 0};
unsigned long lastPressTime[5] = {0, 0, 0, 0, 0};

bool switch_light_state()
{
    return digitalRead(SWITCH_LIGHT_PIN) == HIGH;
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

unsigned long getLockTime(Button button)
{
    if (button == BUTTON_PLUS || button == BUTTON_MINUS)
        return LOCK_TIME;

    return 0;
}

void buttons_init()
{
    pinMode(SWITCH_LIGHT_PIN, INPUT_PULLUP);
    pinMode(BUTTON_PLUS_PIN, INPUT_PULLUP);
    pinMode(BUTTON_MINUS_PIN, INPUT_PULLUP);
    pinMode(BUTTON_VALIDATE_PIN, INPUT_PULLUP);
    pinMode(BUTTON_PARAM_PIN, INPUT_PULLUP);

    unsigned long now = millis();

    for (int i = 0; i < 5; i++)
    {
        bool state = digitalRead(getButtonPin((Button)i)) == LOW;

        rawState[i] = state;
        stableState[i] = state;
        lastPressTime[i] = now - getLockTime((Button)i);
    }
}

void buttons_update()
{
    unsigned long now = millis();

    for (int i = 0; i < 5; i++)
    {
        Button button = (Button)i;

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

                if (stableState[i])                {
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


bool button_pressed(Button button)
{
    return stableState[button];
}


bool button_just_pressed(Button button)
{
    if (justPressedState[button])
    {
        justPressedState[button] = false;
        return true;
    }

    return false;
}