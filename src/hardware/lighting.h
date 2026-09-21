#ifndef LIGHTING_H
#define LIGHTING_H

#include <Arduino.h>
#include <FastLED.h>
#include "config.h"

void lighting_init();

void lighting_on(CRGB color);
void lighting_off();
bool lighting_is_on();

void change_color(CRGB color);

void increase_brightness();
void decrease_brightness();
int  brightness();



#endif