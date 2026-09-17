#include <Arduino.h>
#include "config.h"
#include "lighting/lighting.h"
#include "hardware/button.h"
#include "modes/light_mode.h"
#include "hardware/screen.h"
#include "hardware/rtc.h"

void setup()
{
    Serial.begin(115200);

    rtc_init();
    buttons_init();
    lighting_init();
    light_mode_init();
    screen_init();

    Serial.println("================================");
    Serial.println("        Sunrise Simulator       ");
    Serial.println("================================");
}


void loop()
{
    buttons_update();
    backlight_update();
    light_mode_update();
    rtc_update();
    
    static int lastMinute = -1;

    int hour = rtc_get_hour();
    int minute = rtc_get_minute();

    if (minute != lastMinute)
    {
        lastMinute = minute;

        screen_show_home( hour, minute, true, false, true, false, false);
    }
}