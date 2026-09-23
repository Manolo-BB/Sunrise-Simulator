#include <Arduino.h>
#include "alarm.h"

#include "../hardware/button.h"
#include "../hardware/screen.h"

// Alarm configuration
static const uint8_t NUMBER_OF_ALARMS = 5;
// Maximum rising time in minutes
static const uint8_t MAX_RISING_TIME = 45;

struct Alarm
{
    uint8_t hour;
    uint8_t minute;
    uint8_t risingTime;
};

static Alarm alarms[NUMBER_OF_ALARMS] =
{
    {7, 0, 30},
    {8, 0, 30},
    {9, 0, 30},
    {10, 0, 30},
    {11, 0, 30}
};

// Alarm selection state
static uint8_t selectedAlarm = 0;
static bool selectionFinished = false;

// Alarm setting state
static AlarmSettingStep settingStep = ALARM_SET_HOUR;
static bool settingFinished = false;

static uint8_t settingHour = 0;
static uint8_t settingMinute = 0;
static uint8_t settingRisingTime = 0;

// Blink management
static unsigned long lastBlinkTime = 0;
static bool blinkOn = true;

#define BLINK_INTERVAL 500

// Initialization
void alarm_init()
{
    selectedAlarm = 0;
    selectionFinished = false;

    settingStep = ALARM_SET_HOUR;
    settingFinished = false;
}

// Start the alarm selection screen
void alarm_start_setting()
{
    selectedAlarm = 0;
    selectionFinished = false;
    screen_reset_alarm_page();

     // Clear all pending button events before entering the alarm selection page
    button_clear_plus_minus_count();
    button_clear_validate_count();

    screen_show_alarm_setting_page(selectedAlarm);
}

// Handle alarm selection buttons*
void alarm_update()
{
    // Move to the next alarm
    if (button_just_pressed(BUTTON_PLUS))
    {
        if (selectedAlarm == 0)
            selectedAlarm = NUMBER_OF_ALARMS - 1;
        else
            selectedAlarm--;

        screen_show_alarm_setting_page(selectedAlarm);
    }

    // Move to the previous alarm
    if (button_just_pressed(BUTTON_MINUS))
    {
        selectedAlarm++;

        if (selectedAlarm >= NUMBER_OF_ALARMS)
            selectedAlarm = 0;

        screen_show_alarm_setting_page(selectedAlarm);
    }

    // Validate the selected alarm
    if (button_just_pressed(BUTTON_VALIDATE))
    {
        selectionFinished = true;
        
        // Clear interrupts from the previous screen
        button_clear_plus_minus_count();
        button_clear_validate_count();
    }
}

// Status
bool alarm_selection_finished()
{
    return selectionFinished;
}

uint8_t alarm_get_selected()
{
    return selectedAlarm;
}

// Start alarm setting
void alarm_start_alarm_setting()
{
    Alarm &alarm = alarms[selectedAlarm];

    settingHour = alarm.hour;
    settingMinute = alarm.minute;
    settingRisingTime = alarm.risingTime;

    settingStep = ALARM_SET_HOUR;
    settingFinished = false;

    blinkOn = true;
    lastBlinkTime = millis();

    button_clear_plus_minus_count();
    button_clear_validate_count();

    screen_show_alarm_time_setting_page( settingHour, settingMinute);
}

// Alarm setting
void alarm_setting_update()
{
    uint8_t plusCount = button_get_plus_count();
    uint8_t minusCount = button_get_minus_count();
    uint8_t validateCount = button_get_validate_count();

    bool valueChanged = (plusCount > 0 || minusCount > 0);

    // Blink the field currently being edited
    unsigned long now = millis();

    if (now - lastBlinkTime >= BLINK_INTERVAL)
    {
        lastBlinkTime = now;
        blinkOn = !blinkOn;

        // Hour
        if (settingStep == ALARM_SET_HOUR)
        {
            screen_update_alarm_setting_hour(settingHour, blinkOn);
        }

        // Minute
        else if (settingStep == ALARM_SET_MINUTE)
        {
            screen_update_alarm_setting_minute(settingMinute, blinkOn);
        }
    }

    // Hour
    if (settingStep == ALARM_SET_HOUR)
    {
        while (plusCount--)
        {
            if (settingHour >= 23)
                settingHour = 0;
            else
                settingHour++;
        }

        while (minusCount--)
        {
            if (settingHour == 0)
                settingHour = 23;
            else
                settingHour--;
        }

        if (valueChanged)
        {
            screen_update_alarm_setting_hour(settingHour, true);
        }

        if (validateCount > 0)
        {
            // Clear button interrupts before changing screen
            button_clear_plus_minus_count();
            button_clear_validate_count();

            settingStep = ALARM_SET_MINUTE;

            blinkOn = true;
            lastBlinkTime = millis();

            screen_update_alarm_setting_hour(settingHour, true);
            screen_update_alarm_setting_minute(settingMinute, true);
        }

        return;
    }

    // Minute
    if (settingStep == ALARM_SET_MINUTE)
    {
        while (plusCount--)
        {
            if (settingMinute >= 59)
                settingMinute = 0;
            else
                settingMinute++;
        }

        while (minusCount--)
        {
            if (settingMinute == 0)
                settingMinute = 59;
            else
                settingMinute--;
        }

        if (valueChanged)
        {
            screen_update_alarm_setting_minute(settingMinute, true);
        }

        if (validateCount > 0)
        {
            // Clear button interrupts before changing screen
            button_clear_plus_minus_count();
            button_clear_validate_count();

            screen_update_alarm_setting_minute(settingMinute, true);

            settingStep = ALARM_SET_RISING_TIME;

            blinkOn = true;
            lastBlinkTime = millis();

            screen_show_rising_time_setting_page(settingRisingTime);
        }
        return;
    }

    // Rising time
    if (settingStep == ALARM_SET_RISING_TIME)
    {
        while (plusCount--)
        {
            if (settingRisingTime < MAX_RISING_TIME)
                settingRisingTime++;
        }

        while (minusCount--)
        {
            if (settingRisingTime > 0)
                settingRisingTime--;
        }

        if (valueChanged)
        {
            screen_update_rising_time_setting(settingRisingTime);
        }

        if (validateCount > 0)
        {
            screen_update_rising_time_setting(settingRisingTime);

            alarms[selectedAlarm].hour = settingHour;
            alarms[selectedAlarm].minute = settingMinute;
            alarms[selectedAlarm].risingTime = settingRisingTime;

            button_clear_plus_minus_count();
            button_clear_validate_count();

            settingFinished = true;
        }
        return;
    }
}

// Status
bool alarm_setting_finished()
{
    return settingFinished;
}

// Getters
uint8_t alarm_get_setting_hour()
{
    return settingHour;
}

uint8_t alarm_get_setting_minute()
{
    return settingMinute;
}

uint8_t alarm_get_setting_rising_time()
{
    return settingRisingTime;
}