#ifndef LIGHTING_H
#define LIGHTING_H

#include <Arduino.h>
#include <FastLED.h>

void lighting_init();
void lighting_update();

void lighting_on(CRGB color);
void lighting_off();

void change_color(CRGB color);

void increase_brightness();
void decrease_brightness();
int  brightness();

bool lighting_is_on();


#endif