#ifndef BUTTONS_H
#define BUTTONS_H

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

bool switch_light_state();

//Getters and setters
bool button_pressed(Button button);
bool button_just_pressed(Button button);
void button_clear_press(Button button);

int getButtonPin(Button button);
unsigned long getLockTime(Button button);

uint8_t button_get_plus_count();
uint8_t button_get_minus_count();
uint8_t button_get_validate_count();

//Resets counters
void button_clear_plus_minus_count();
void button_clear_validate_count();

#endif