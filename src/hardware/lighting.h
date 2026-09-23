#ifndef LIGHTING_H
#define LIGHTING_H

#include <FastLED.h>

void lighting_init();

//Light control
void lighting_on(CRGB color);
void lighting_off();
void change_color(CRGB color);
bool lighting_is_on();

//Functions for checking and adjusting the brightness
void increase_brightness();
void decrease_brightness();
int  brightness();

#endif