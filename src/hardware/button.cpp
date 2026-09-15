#include <Arduino.h>
#include "button.h"
#include "config.h"

#define DEBOUNCE_TIME 30

// Temps minimum entre deux appuis validés
#define SWITCH_LOCK_TIME 250
#define ADJUST_LOCK_TIME 150

bool rawState[4] = {false, false, false, false};
bool stableState[4] = {false, false, false, false};
bool justPressedState[4] = {false, false, false, false};

unsigned long lastChangeTime[4] = {0, 0, 0, 0};
unsigned long lastPressTime[4] = {0, 0, 0, 0};

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
    }

    return -1;
}


unsigned long getLockTime(Button button)
{
    if (button == SWITCH_LIGHT)
        return SWITCH_LOCK_TIME;

    if (button == BUTTON_PLUS || button == BUTTON_MINUS)
        return ADJUST_LOCK_TIME;

    return 0;
}


void buttons_init()
{
    pinMode(SWITCH_LIGHT_PIN, INPUT_PULLUP);
    pinMode(BUTTON_PLUS_PIN, INPUT_PULLUP);
    pinMode(BUTTON_MINUS_PIN, INPUT_PULLUP);
    pinMode(BUTTON_VALIDATE_PIN, INPUT_PULLUP);

    unsigned long now = millis();

    for (int i = 0; i < 4; i++)
    {
        bool state = digitalRead(getButtonPin((Button)i)) == LOW;

        rawState[i] = state;
        stableState[i] = state;

        // Permet au premier appui d'être immédiatement accepté
        lastPressTime[i] = now - getLockTime((Button)i);
    }
}


void buttons_update()
{
    unsigned long now = millis();

    for (int i = 0; i < 4; i++)
    {
        Button button = (Button)i;

        bool currentRaw = digitalRead(getButtonPin(button)) == LOW;

        // Détection d'un changement brut
        if (currentRaw != rawState[i])
        {
            rawState[i] = currentRaw;
            lastChangeTime[i] = now;
        }

        // Le changement doit être stable pendant DEBOUNCE_TIME
        if ((now - lastChangeTime[i]) >= DEBOUNCE_TIME)
        {
            if (stableState[i] != rawState[i])
            {
                stableState[i] = rawState[i];

                // Nouveau appui
                if (stableState[i])
                {
                    unsigned long lockTime = getLockTime(button);

                    // Vérifie le délai depuis le dernier appui accepté
                    if ((now - lastPressTime[i]) >= lockTime)
                    {
                        justPressedState[i] = true;
                        lastPressTime[i] = now;
                    }
                    else
                    {
                        // Appui ignoré
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