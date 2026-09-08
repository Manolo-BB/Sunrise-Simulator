#ifndef BUTTONS_H
#define BUTTONS_H

enum Button
{
    SWITCH_LIGHT,
    BUTTON_PLUS ,
    BUTTON_MINUS,
    BUTTON_VALIDATE
};

void buttons_init();

void buttons_update();

bool button_pressed(Button button);

// Retourne true lors d'un nouvel appui
bool button_just_pressed(Button button);

// Retourne true lorsque le bouton est maintenu
bool button_is_held(Button button);

#endif