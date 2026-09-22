#ifndef LIGHTING_H
#define LIGHTING_H

//Global libraries
#include <Arduino.h>
#include <FastLED.h>
#include "config.h"

#define NUM_LEDS    67 //Number of Leds on the strip
#define BRIGHTNESS  30 //Initialisation brightness at first power on

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