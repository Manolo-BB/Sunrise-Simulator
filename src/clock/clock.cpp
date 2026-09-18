#include "clock.h"

static ClockSettingStep settingStep = CLOCK_SET_HOUR;

static uint8_t settingHour = 0;
static uint8_t settingMinute = 0;

static uint8_t settingDay = 1;
static uint8_t settingMonth = 1;
static uint16_t settingYear = 2026;

static bool settingFinished = false;

static unsigned long lastBlinkTime = 0;
static bool blinkOn = true;

void clock_init()
{
    settingFinished = false;
}

void clock_start_time_setting()
{
    settingHour = rtc_get_hour();
    settingMinute = rtc_get_minute();

    settingStep = CLOCK_SET_HOUR;
    settingFinished = false;
    blinkOn = true;
    lastBlinkTime = millis();
}

void clock_update()
{
    if (millis() - lastBlinkTime >= BLINK_INTERVAL)
    {
        lastBlinkTime = millis();
        blinkOn = !blinkOn;
    }
}

void clock_time_setting_update()
{
    if (button_just_pressed(BUTTON_PLUS))
    {
        Serial.println("PLUS");

        if (settingStep == CLOCK_SET_HOUR)
        {
            settingHour++;

            if (settingHour >= 24)
                settingHour = 0;
            
            screen_update_setting_hour(settingHour);
        }
        else if (settingStep == CLOCK_SET_MINUTE)
        {
            settingMinute++;

            if (settingMinute >= 60)
                settingMinute = 0;

            screen_update_setting_minute(settingMinute);
        }
    }

    if (button_just_pressed(BUTTON_MINUS))
    {
        Serial.println("MINUS");

        if (settingStep == CLOCK_SET_HOUR)
        {
            if (settingHour == 0)
                settingHour = 23;
            else
                settingHour--;
            
            screen_update_setting_hour(settingHour);
        }
        else if (settingStep == CLOCK_SET_MINUTE)
        {
            if (settingMinute == 0)
                settingMinute = 59;
            else
                settingMinute--;

            screen_update_setting_minute(settingMinute);
        }
    }
    if (button_just_pressed(BUTTON_VALIDATE))
    {
        if (settingStep == CLOCK_SET_HOUR)
        {
            settingStep = CLOCK_SET_MINUTE;

            blinkOn = true;
            lastBlinkTime = millis();
        }
        else if (settingStep == CLOCK_SET_MINUTE)
        {
            rtc_set_time(settingHour, settingMinute);

            settingStep = CLOCK_SET_DATE;

            blinkOn = true;
            lastBlinkTime = millis();
        }
    }
}
bool clock_time_setting_finished()
{
    return settingFinished;
}

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

bool clock_is_blink_on()
{
    return blinkOn;
}

bool clock_setting_hour_active()
{
    return settingStep == CLOCK_SET_HOUR;
}

bool clock_setting_minute_active()
{
    return settingStep == CLOCK_SET_MINUTE;
}