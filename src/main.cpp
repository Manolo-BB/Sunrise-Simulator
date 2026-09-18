#include <Arduino.h>
#include "config.h"
#include "lighting/lighting.h"
#include "hardware/button.h"
#include "modes/light_mode.h"
#include "hardware/screen.h"
#include "hardware/rtc.h"
#include "modes/menu_mode.h"
#include "clock/clock.h"

static int lastDisplayedHour = -1;
static int lastDisplayedMinute = -1;
static bool wasMenuActive = false;

void setup()
{
    Serial.begin(115200);

    rtc_init();
    buttons_init();
    light_mode_init();
    screen_init();
    screen_show_home_complete(rtc_get_hour(), rtc_get_minute(), false, false, true, false, false);
    menu_init();

    Serial.println("================================");
    Serial.println("        Sunrise Simulator       ");
    Serial.println("================================");
}

void loop()
{
    buttons_update();
    backlight_update();
    if (!menu_is_active())
    {
        light_mode_update();
    }
    rtc_update();
    clock_update();
    menu_update();

    int hour = rtc_get_hour();
    int minute = rtc_get_minute();

    if (wasMenuActive && !menu_is_active()){
        screen_show_home_complete(hour, minute, false, false, true, false, false);
        lastDisplayedHour = hour;
        lastDisplayedMinute = minute;
    }

    if (!menu_is_active())
    {
        if (hour != lastDisplayedHour || minute != lastDisplayedMinute)
        {
            lastDisplayedHour = hour;
            lastDisplayedMinute = minute;
            screen_show_home(hour, minute);
        }
    }
    wasMenuActive=menu_is_active();
}