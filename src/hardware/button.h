#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>
#include "config.h"

enum Button
{
    SWITCH_LIGHT,
    BUTTON_PLUS ,
    BUTTON_MINUS,
    BUTTON_VALIDATE,
    BUTTON_PARAM
};

void buttons_init();
void buttons_update();
bool button_pressed(Button button);
bool button_just_pressed(Button button);
void button_clear_press(Button button);
bool button_is_held(Button button);
bool switch_light_state();

#endif