//Global libraries
#include <Arduino.h>
#include "config.h"

//Hardware functions
#include "hardware/lighting.h"
#include "hardware/button.h"
#include "hardware/screen.h"
#include "hardware/rtc.h"

//Modes functions
#include "modes/light_mode.h"
#include "modes/menu_mode.h"
#include "modes/clock.h"

//Variables
static int lastDisplayedHour = -1;
static int lastDisplayedMinute = -1;
static int lastDisplayedDay = -1;
static int lastDisplayedMonth = -1;
static int lastDisplayedYear = -1;

static bool wasMenuActive = false;

//Function for configuring all hardware and modes run once at first power on
void setup()
{
    //Begin serial communication for debugging
    Serial.begin(115200);

    //Hardware initialisation
    rtc_init();
    buttons_init();
    screen_init();

    //Modes initialisation
    light_mode_init();
    menu_init();

    //Initial display
    screen_show_home_complete(rtc_get_hour(), rtc_get_minute(), false, false, true, false, false);

    Serial.println("================================");
    Serial.println("        Sunrise Simulator       ");
    Serial.println("================================");
}

//Main function that runs continuously
void loop()
{   
    //Reading the actions on the buttons
    buttons_update();
    backlight_update();
    
    //We only adjust the colour and brightness of the light when the menu is not active (the ‘+’ and ‘-’ buttons are used for other functions within the menu)
    if (!menu_is_active())
    {
        light_mode_update();
    }
    
    //We increment the software clock and update the internal variables
    rtc_update();
    clock_update();
    menu_update();

    //We collect the information before it is displayed
    int hour = rtc_get_hour();
    int minute = rtc_get_minute();
    uint8_t day = rtc_get_day();
    uint8_t month = rtc_get_month();
    uint16_t year = rtc_get_year();

    //Depending on the values of the internal variables, we adjust the display
    //If the user exits the menu, we redraw everything
    if (wasMenuActive && !menu_is_active()){
        screen_show_home_complete(hour, minute, false, false, true, false, false);
        lastDisplayedHour = hour;
        lastDisplayedMinute = minute;
    }

    //If the time has changed, we only update the time
    if (!menu_is_active())
    {
        if (hour != lastDisplayedHour || minute != lastDisplayedMinute)
        {
            lastDisplayedHour = hour;
            lastDisplayedMinute = minute;
            screen_show_home(hour, minute);
        }
    }

    //If the date has changed, we only update the date
    if (day != lastDisplayedDay || month != lastDisplayedMonth || year != lastDisplayedYear)
    {
        screen_update_home_date();

        lastDisplayedDay = day;
        lastDisplayedMonth = month;
        lastDisplayedYear = year;
}
    wasMenuActive=menu_is_active();
}