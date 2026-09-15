#include <Arduino.h>
#include "config.h"
#include "lighting/lighting.h"
#include "hardware/button.h"
#include "modes/light_mode.h"
#include "hardware/screen.h"


void setup()
{
    Serial.begin(115200);

    //buttons_init();
    //lighting_init();
    //light_mode_init();
    screen_init();

    Serial.println("================================");
    Serial.println("       ALARM CLOCK TEST");
    Serial.println("================================");
}


void loop()
{
    //light_mode_update();
    fake_screen();
}