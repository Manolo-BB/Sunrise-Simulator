#include <Arduino.h>
#include "alarm.h"

#include "../hardware/button.h"
#include "../hardware/screen.h"
#include "../hardware/audio.h"

struct Alarm
{
    uint8_t hour;
    uint8_t minute;
    uint8_t risingTime;
    uint8_t sound;
    bool monday;
    bool tuesday;
    bool wednesday;
    bool thursday;
    bool friday;
    bool saturday;
    bool sunday;
    bool enabled;
};

static Alarm alarms[NUMBER_OF_ALARMS] =
{
    {7,  0, 30, 1, true, true,  true,  true,  true,  false, false, false},
    {7,  0, 30, 2, true, true,  true,  true,  true,  false, false, false},
    {7,  0, 30, 3, true, true,  true,  true,  true,  false, false, false},
    {7,  0, 30, 4, true, true,  true,  true,  true,  false, false, false},
    {7,  0, 30, 5, true, true,  true,  true,  true,  false, false, false},
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
static uint8_t settingSound = 1;
static bool settingMonday = false;
static bool settingTuesday = false;
static bool settingWednesday = false;
static bool settingThursday = false;
static bool settingFriday = false;
static bool settingSaturday = false;
static bool settingSunday = false;
static bool settingEnabled = false;

// Blink management
static unsigned long lastBlinkTime = 0;
static bool blinkOn = true;

// Setting days management
static uint8_t selectedDay = 0;

static unsigned long validatePressStartTime = 0;
static bool validateLongHandled = false;

#define VALIDATE_LONG_PRESS_TIME 1000

#define BLINK_INTERVAL 500

static void alarm_start_hour_setting();
static void alarm_update_hour_setting();

static void alarm_start_minute_setting();
static void alarm_update_minute_setting();

static void alarm_start_rising_time_setting();
static void alarm_update_rising_time_setting();

static void alarm_start_sound_setting();
static void alarm_update_sound_setting();

static void alarm_start_days_setting();
static void alarm_update_days_setting();
static bool* alarm_get_selected_day_state();

static void alarm_start_enabled_setting();
static void alarm_update_enabled_setting();

static void alarm_save_settings();

static void alarm_start_hour_setting()
{
    blinkOn = true;
    lastBlinkTime = millis();

    button_clear_all();

    screen_show_alarm_time_setting_page( settingHour,settingMinute );
}

static void alarm_update_hour_setting()
{
    uint8_t plusCount = button_get_plus_count();
    uint8_t minusCount = button_get_minus_count();
    uint8_t validateCount = button_get_validate_count();

    bool valueChanged = (plusCount > 0 || minusCount > 0);

    // Blink hour
    unsigned long now = millis();

    if (now - lastBlinkTime >= BLINK_INTERVAL)
    {
        lastBlinkTime = now;
        blinkOn = !blinkOn;

        screen_update_alarm_setting_hour( settingHour, blinkOn);
    }

    // Increase hour
    while (plusCount--)
    {
        if (settingHour >= 23)
            settingHour = 0;
        else
            settingHour++;
    }

    // Decrease hour
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

    // Validate hour
    if (validateCount > 0)
    {
        button_clear_all();

        settingStep = ALARM_SET_MINUTE;

        screen_update_alarm_setting_hour(settingHour,true);

        alarm_start_minute_setting();
    }
}

// Minutes settings
static void alarm_start_minute_setting()
{
    blinkOn = true;
    lastBlinkTime = millis();

    button_clear_plus_minus_count();
    button_clear_validate_count();

    screen_show_alarm_time_setting_page(settingHour, settingMinute);
}

static void alarm_update_minute_setting()
{
    uint8_t plusCount = button_get_plus_count();
    uint8_t minusCount = button_get_minus_count();
    uint8_t validateCount = button_get_validate_count();

    bool valueChanged = (plusCount > 0 || minusCount > 0);

    // Blink minute
    unsigned long now = millis();

    if (now - lastBlinkTime >= BLINK_INTERVAL)
    {
        lastBlinkTime = now;
        blinkOn = !blinkOn;

        screen_update_alarm_setting_minute( settingMinute, blinkOn);
    }

    // Increase minute
    while (plusCount--)
    {
        if (settingMinute >= 59)
            settingMinute = 0;
        else
            settingMinute++;
    }

    // Decrease minute
    while (minusCount--)
    {
        if (settingMinute == 0)
            settingMinute = 59;
        else
            settingMinute--;
    }

    if (valueChanged)
    {
        screen_update_alarm_setting_minute(settingMinute,true);
    }

    // Validate minute
    if (validateCount > 0)
    {
        button_clear_all();

        settingStep = ALARM_SET_RISING_TIME;

        screen_update_alarm_setting_minute(settingMinute, true);

        alarm_start_rising_time_setting();
    }
}

//Rising time settings
static void alarm_start_rising_time_setting()
{
    button_clear_plus_minus_count();
    button_clear_validate_count();

    screen_show_rising_time_setting_page( settingRisingTime);
}


static void alarm_update_rising_time_setting()
{
    uint8_t plusCount = button_get_plus_count();
    uint8_t minusCount = button_get_minus_count();
    uint8_t validateCount = button_get_validate_count();

    bool valueChanged = (plusCount > 0 || minusCount > 0);

    // Increase rising time
    while (plusCount--)
    {
        if (settingRisingTime < MAX_RISING_TIME)
            settingRisingTime++;
    }

    // Decrease rising time
    while (minusCount--)
    {
        if (settingRisingTime > 0)
            settingRisingTime--;
    }

    if (valueChanged)
    {
        screen_update_rising_time_setting( settingRisingTime);
    }

    // Validate rising time
    if (validateCount > 0)
    {
        button_clear_all();

        screen_update_rising_time_setting( settingRisingTime);

        settingStep = ALARM_SET_SOUND;

        alarm_start_sound_setting();
    }
}

//Sound settings
static void alarm_start_sound_setting()
{
    button_clear_all();

    screen_show_sound_setting_page( settingSound);
    // Play the currently selected sound
    audio_play_sound(settingSound);
}

static void alarm_update_sound_setting()
{
    if (button_just_pressed(BUTTON_MINUS))
    {
        if (settingSound >= 5)
            settingSound = 1;
        else
            settingSound++;

        screen_update_sound_setting(settingSound);
        audio_play_sound(settingSound);
    }

    if (button_just_pressed(BUTTON_PLUS))
    {
        if (settingSound <= 1)
            settingSound = 5;
        else
            settingSound--;

        screen_update_sound_setting(settingSound);
        audio_play_sound(settingSound);
    }

    if (button_just_pressed(BUTTON_VALIDATE))
    {
        audio_stop();
        button_clear_all();
        
        settingStep = ALARM_SET_DAYS;

        alarm_start_days_setting();
    }
}

//Days settings
static void alarm_start_days_setting()
{
    button_clear_all();

    //Start on Monday
    selectedDay = 0;
    // Reset long-press detection
    validatePressStartTime = 0;
    validateLongHandled = false;

    screen_show_days_setting_page( settingMonday, settingTuesday, settingWednesday, settingThursday, settingFriday, settingSaturday,settingSunday, selectedDay);
}


static void alarm_update_days_setting()
{
    if (button_just_pressed(BUTTON_PLUS))
    {
        selectedDay++;

        if (selectedDay >= 7)
            selectedDay = 0;

        screen_update_days_setting( settingMonday, settingTuesday, settingWednesday, settingThursday, settingFriday, settingSaturday,settingSunday, selectedDay);
    }

    if (button_just_pressed(BUTTON_MINUS))
    {
        if (selectedDay == 0)
            selectedDay = 6;
        else
            selectedDay--;

        screen_update_days_setting( settingMonday, settingTuesday, settingWednesday, settingThursday, settingFriday, settingSaturday,settingSunday, selectedDay);
    }

    // Detect the beginning of a VALIDATE press
    if (button_pressed(BUTTON_VALIDATE) &&  validatePressStartTime == 0)
    {
        validatePressStartTime = millis();
        validateLongHandled = false;
    }

    // Detect a long VALIDATE press
    if (button_pressed(BUTTON_VALIDATE) && !validateLongHandled &&  millis() - validatePressStartTime >= VALIDATE_LONG_PRESS_TIME)
    {
        validateLongHandled = true;

        button_clear_all();

        settingStep = ALARM_SET_ENABLED;

        alarm_start_enabled_setting();
    }

    // Detect the release of VALIDATE after a short press
    if (!button_pressed(BUTTON_VALIDATE) && validatePressStartTime != 0)
    {
        unsigned long pressDuration = millis() - validatePressStartTime;

        if (!validateLongHandled && pressDuration < VALIDATE_LONG_PRESS_TIME)
        {
            bool* selectedState = alarm_get_selected_day_state();

            if (selectedState != nullptr)
            {
                *selectedState = !(*selectedState);
            }
            screen_update_days_setting( settingMonday, settingTuesday, settingWednesday, settingThursday, settingFriday, settingSaturday,settingSunday, selectedDay);
        }
        validatePressStartTime = 0;
        validateLongHandled = false;
    }
}

static bool* alarm_get_selected_day_state()
{
    switch (selectedDay)
    {
        case 0: return &settingMonday;
        case 1: return &settingTuesday;
        case 2: return &settingWednesday;
        case 3: return &settingThursday;
        case 4: return &settingFriday;
        case 5: return &settingSaturday;
        case 6: return &settingSunday;
    }

    return nullptr;
}

// Enabled settings
static void alarm_start_enabled_setting()
{
    button_clear_plus_minus_count();
    button_clear_validate_count();

    screen_show_enabled_setting_page( settingEnabled);
}


static void alarm_update_enabled_setting()
{
    if (button_just_pressed(BUTTON_PLUS) ||
        button_just_pressed(BUTTON_MINUS))
    {
        settingEnabled = !settingEnabled;

        screen_update_enabled_setting( settingEnabled);
    }

    if (button_just_pressed(BUTTON_VALIDATE))
    {
        button_clear_all();

        alarm_save_settings();

        settingFinished = true;
    }
}

// Save alarm settings
static void alarm_save_settings()
{
    Alarm &alarm = alarms[selectedAlarm];

    alarm.hour = settingHour;
    alarm.minute = settingMinute;
    alarm.risingTime = settingRisingTime;
    alarm.sound = settingSound;
    alarm.monday = settingMonday;
    alarm.tuesday = settingTuesday;
    alarm.wednesday = settingWednesday;
    alarm.thursday = settingThursday;
    alarm.friday = settingFriday;
    alarm.saturday = settingSaturday;
    alarm.sunday = settingSunday;
    alarm.enabled = settingEnabled;
}

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
    button_clear_all();

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
        button_clear_all();
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

    //Copy current values into temporary settings
    settingHour = alarm.hour;
    settingMinute = alarm.minute;
    settingRisingTime = alarm.risingTime;
    settingSound = alarm.sound;
    settingMonday = alarm.monday;
    settingTuesday = alarm.tuesday;
    settingWednesday = alarm.wednesday;
    settingThursday = alarm.thursday;
    settingFriday = alarm.friday;
    settingSaturday = alarm.saturday;
    settingSunday = alarm.sunday;
    settingEnabled = alarm.enabled;
    
    settingStep = ALARM_SET_HOUR;
    settingFinished = false;

    button_clear_all();

    alarm_start_hour_setting();
}

// Alarm setting
void alarm_setting_update()
{
    switch (settingStep)
    {
        case ALARM_SET_HOUR:
            alarm_update_hour_setting();
            break;

        case ALARM_SET_MINUTE:
            alarm_update_minute_setting();
            break;

        case ALARM_SET_RISING_TIME:
            alarm_update_rising_time_setting();
            break;

        case ALARM_SET_SOUND:
            alarm_update_sound_setting();
            break;

        case ALARM_SET_DAYS:
            alarm_update_days_setting();
            break;

        case ALARM_SET_ENABLED:
            alarm_update_enabled_setting();
            break;
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

uint8_t alarm_get_setting_sound()
{
    return settingSound;
}

bool alarm_is_enabled(uint8_t alarmIndex)
{
    if (alarmIndex >= NUMBER_OF_ALARMS)
        return false;

    return alarms[alarmIndex].enabled;
}