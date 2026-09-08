#include <Arduino.h>

#include "lighting/lighting.h"
#include "hardware/button.h"
#include "modes/light_mode.h"


void setup()
{
    Serial.begin(115200);

    buttons_init();
    lighting_init();
    light_mode_init();

    Serial.println("================================");
    Serial.println("       ALARM CLOCK TEST");
    Serial.println("================================");
}


void loop()
{
    light_mode_update();
}