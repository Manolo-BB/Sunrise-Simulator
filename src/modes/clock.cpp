#include <Arduino.h>
#include "clock.h"

#include "../hardware/rtc.h"
#include "../hardware/button.h"
#include "../hardware/screen.h"

// Clock configuration
static const unsigned long BLINK_INTERVAL = 1000;

// Clock setting state
static ClockSettingStep settingStep = CLOCK_SET_HOUR;

static uint8_t settingHour = 0;
static uint8_t settingMinute = 0;

static uint8_t settingDay = 1;
static uint8_t settingMonth = 1;
static uint16_t settingYear = 2026;

static bool settingFinished = false;

// Blinking state
static unsigned long lastBlinkTime = 0;

static bool blinkOn = true;
static bool lastBlinkOn = true;

// ==============================
// Internal functions
// ==============================

// Reset the blinking timer and force the current field to be visible
static void reset_blink()
{
    blinkOn = true;
    lastBlinkOn = true;
    lastBlinkTime = millis();
}

// Return the number of days in the selected month
static uint8_t days_in_month(uint8_t month, uint16_t year)
{
    switch (month)
    {
        case 2:
            // Leap year
            if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
                return 29;

            return 28;

        case 4:
        case 6:
        case 9:
        case 11:
            return 30;

        default:
            return 31;
    }
}

// Apply the button counters to the current setting
static void update_setting_value( uint8_t plusCount,uint8_t minusCount)
{
    switch (settingStep)
    {
        case CLOCK_SET_HOUR:
        {
            settingHour = (settingHour + plusCount) % 24;
            int newHour = (int)settingHour - minusCount;
            newHour %= 24;

            if (newHour < 0)
                newHour += 24;

            settingHour = newHour;

            break;
        }

        case CLOCK_SET_MINUTE:
        {
            settingMinute = (settingMinute + plusCount) % 60;
            int newMinute = (int)settingMinute - minusCount;
            newMinute %= 60;

            if (newMinute < 0)
                newMinute += 60;

            settingMinute = newMinute;

            break;
        }

        case CLOCK_SET_DAY:
        {
            uint8_t maxDay = days_in_month(settingMonth,settingYear );
            int newDay = (int)settingDay + plusCount - minusCount;
            newDay = ((newDay - 1) % maxDay + maxDay) % maxDay + 1;
            settingDay = newDay;

            break;
        }

        case CLOCK_SET_MONTH:
        {
            int newMonth = (int)settingMonth + plusCount - minusCount;
            newMonth = ((newMonth - 1) % 12 + 12) % 12 + 1;
            settingMonth = newMonth;

            // Adjust the day if the new month has fewer days
            uint8_t maxDay = days_in_month( settingMonth, settingYear);

            if (settingDay > maxDay)
                settingDay = maxDay;

            break;
        }

        case CLOCK_SET_YEAR:
        {
            int newYear = (int)settingYear + plusCount - minusCount;

            if (newYear < 2000)
                newYear = 2000;

            settingYear = newYear;

            // Adjust February 29 when changing the year
            uint8_t maxDay = days_in_month( settingMonth, settingYear);

            if (settingDay > maxDay)
                settingDay = maxDay;

            break;
        }
    }
}

// Update the display for the current setting field
static void update_setting_display(bool show)
{
    switch (settingStep)
    {
        case CLOCK_SET_HOUR:
            screen_update_setting_hour( settingHour, show);
            break;

        case CLOCK_SET_MINUTE:
            screen_update_setting_minute(settingMinute,show);
            break;

        case CLOCK_SET_DAY:
            screen_update_setting_date(settingDay, settingMonth,settingYear,0,show);
            break;

        case CLOCK_SET_MONTH:
            screen_update_setting_date(settingDay, settingMonth, settingYear,1, show);
            break;

        case CLOCK_SET_YEAR:
            screen_update_setting_date( settingDay, settingMonth, settingYear,2,show);
            break;
    }
}

// Move to the next setting field
static void next_setting_step()
{
    switch (settingStep)
    {
        case CLOCK_SET_HOUR:
            settingStep = CLOCK_SET_MINUTE;
            break;

        case CLOCK_SET_MINUTE:
            settingStep = CLOCK_SET_DAY;
            break;

        case CLOCK_SET_DAY:
            settingStep = CLOCK_SET_MONTH;
            break;

        case CLOCK_SET_MONTH:
            settingStep = CLOCK_SET_YEAR;
            break;

        case CLOCK_SET_YEAR:
            rtc_set_time(settingHour, settingMinute);
            rtc_set_date( settingDay,settingMonth,settingYear);
            settingFinished = true;
            break;
    }

    reset_blink();

    // Refresh the newly selected field
    update_setting_display(true);
}

// Refresh the display when the blinking state changes
static void update_blink_display()
{
    if (blinkOn == lastBlinkOn)
        return;

    update_setting_display(blinkOn);

    lastBlinkOn = blinkOn;
}

// ==================================
// Initialization
// ==================================
void clock_init()
{
    settingFinished = false;
}

// Update the software clock and blinking timer
void clock_update()
{
    if (millis() - lastBlinkTime >= BLINK_INTERVAL)
    {
        lastBlinkTime = millis();
        blinkOn = !blinkOn;
    }
}


// Load the current RTC values and start the setting sequence
void clock_start_time_setting()
{
    settingHour = rtc_get_hour();
    settingMinute = rtc_get_minute();

    settingDay = rtc_get_day();
    settingMonth = rtc_get_month();
    settingYear = rtc_get_year();

    settingStep = CLOCK_SET_HOUR;
    settingFinished = false;

    button_clear_plus_minus_count();
    button_clear_validate_count();

    reset_blink();
}

// Process button inputs and update the time/date setting sequence
void clock_time_setting_update()
{
    // Refresh the display when the blinking state changes
    update_blink_display();

    // Read all button presses accumulated during display refresh
    uint8_t plusCount = button_get_plus_count();
    uint8_t minusCount = button_get_minus_count();

    // Update the current value
    if (plusCount > 0 || minusCount > 0)
    {
        update_setting_value( plusCount,minusCount);
        update_setting_display(true);
    }

    // Check whether the validation button was pressed
    uint8_t validateCount = button_get_validate_count();

    if (validateCount > 0)
    {
        next_setting_step();
    }
}

// Return true when the complete time/date setting sequence is finished
bool clock_time_setting_finished()
{
    return settingFinished;
}

// Setting value getters
uint8_t clock_get_setting_hour()
{
    return settingHour;
}

uint8_t clock_get_setting_minute()
{
    return settingMinute;
}

uint8_t clock_get_setting_day()
{
    return settingDay;
}

uint8_t clock_get_setting_month()
{
    return settingMonth;
}

uint16_t clock_get_setting_year()
{
    return settingYear;
}