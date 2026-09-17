#ifndef LIGHT_MODE_H
#define LIGHT_MODE_H

#include <Arduino.h>
#include "config.h"
#include "../lighting/lighting.h"
#include "../hardware/button.h"

#define VALIDATION_TIME 1000
#define NUMBER_OF_COLORS 7

void light_mode_init();
void light_mode_update();
void backlight_update();

#endif