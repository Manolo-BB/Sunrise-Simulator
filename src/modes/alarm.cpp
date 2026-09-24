#include <Arduino.h>
#include "alarm.h"

#include "../hardware/button.h"
#include "../hardware/screen.h"
#include "../hardware/audio.h"
#include "../hardware/rtc.h"
#include "../hardware/lighting.h"

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
    {9,  0, 30, 1, true, true,  true,  true,  true,  false, false, false},
    {10,  0, 30, 2, true, true,  true,  true,  true,  false, false, false},
    {14,  0, 5, 3, true, true,  true,  true,  true,  false, false, false},
    {19,  00, 20, 4, true, true,  true,  true,  true,  false, false, true},
    {18,  55, 30, 5, true, true,  true,  true,  true,  false, false, true},
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

// Sunrise simulation state
static uint8_t currentSunriseBrightness = 0;
static bool sunriseActive = false;

// Indicates whether an alarm occurrence has already been handled today
static bool alarmHandledToday[NUMBER_OF_ALARMS] = {false};
// Indicates whether an alarm is currently ringing
static bool alarmRinging[NUMBER_OF_ALARMS] = {false};
static uint8_t lastAlarmDay = 255;

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

static AlarmState alarm_get_state(uint8_t alarmIndex);

//Sunrise functions
static uint8_t alarm_get_sunrise_brightness(uint8_t alarmIndex);

static void alarm_check_new_day();
static void alarm_start_ringing(uint8_t alarmIndex);
static void alarm_validate(uint8_t alarmIndex);
static void alarm_update_validation();

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
        if (settingRisingTime > MIN_RISING_TIME)
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

bool alarm_is_scheduled_today(uint8_t alarmIndex)
{
    if (alarmIndex >= NUMBER_OF_ALARMS)
        return false;

    if (!alarms[alarmIndex].enabled)
        return false;

    uint8_t day = rtc_get_weekday();

    switch (day)
    {
        case 0: return alarms[alarmIndex].sunday;
        case 1: return alarms[alarmIndex].monday;
        case 2: return alarms[alarmIndex].tuesday;
        case 3: return alarms[alarmIndex].wednesday;
        case 4: return alarms[alarmIndex].thursday;
        case 5: return alarms[alarmIndex].friday;
        case 6: return alarms[alarmIndex].saturday;
    }

    return false;
}

int8_t alarm_get_next_today()
{
    int8_t nextAlarm = -1;
    uint16_t nextAlarmTime = 1440; //The next alarm isn't the same day (24*60=1440)

    uint16_t currentTime =
        rtc_get_hour() * 60 +
        rtc_get_minute();

    for (uint8_t i = 0; i < NUMBER_OF_ALARMS; i++)
    {
        if (!alarm_is_scheduled_today(i))
            continue;

        uint16_t alarmTime =
            alarms[i].hour * 60 +
            alarms[i].minute;

        if (alarmTime < currentTime)
            continue;

        if (alarmTime < nextAlarmTime)
        {
            nextAlarmTime = alarmTime;
            nextAlarm = i;
        }
    }

    return nextAlarm;
}

void alarm_sunrise_update()
{
    alarm_check_new_day();
    alarm_update_validation();

    uint8_t highestBrightness = SUNRISE_START_BRIGHTNESS;
    bool active = false;

    // Check all alarms to find the highest required brightness
    for (uint8_t i = 0; i < NUMBER_OF_ALARMS; i++)
    {
        AlarmState state = alarm_get_state(i);

        if (state == ALARM_SUNRISE)
        {
            active = true;
            uint8_t brightness = alarm_get_sunrise_brightness(i);
            if (brightness > highestBrightness)
            {
                highestBrightness = brightness;
            }
        }
    }

    sunriseActive = active;

    // Apply sunrise brightness
    if (active)
    {
        if (highestBrightness != currentSunriseBrightness)
        {
            currentSunriseBrightness = highestBrightness;

            set_brightness(currentSunriseBrightness);

            Serial.print("Sunrise brightness: ");
            Serial.println(currentSunriseBrightness);
        }

        if (!lighting_is_on())
        {
            lighting_on(CRGB::Yellow);
        }
        else
        {
            FastLED.show();
        }
    }

    // Check for alarms that have reached their wake-up time
    for (uint8_t i = 0; i < NUMBER_OF_ALARMS; i++)
    {
        if (alarm_get_state(i) == ALARM_DUE)
        {
            alarm_start_ringing(i);
        }
    }

    // Turn the light off when no sunrise remains
    if (!active)
    {
        if (lighting_is_on() && !audio_is_playing())
        {
            lighting_off();
        }
    }
}

static uint8_t alarm_get_sunrise_brightness(uint8_t alarmIndex)
{
    uint16_t alarmTime = alarms[alarmIndex].hour * 60 + alarms[alarmIndex].minute;
    uint8_t risingTime = alarms[alarmIndex].risingTime;
    uint16_t currentTime = rtc_get_hour() * 60 + rtc_get_minute();

    int sunriseStartTime = (int)alarmTime - risingTime;
    int elapsedMinutes = currentTime - sunriseStartTime;

    // Handle rising times shorter than the full-light period
    if (risingTime <= SUNRISE_FULL_LIGHT_TIME)
    {
        return SUNRISE_MAX_BRIGHTNESS;
    }

    uint8_t rampMinutes = risingTime - SUNRISE_FULL_LIGHT_TIME;

    // The final five minutes are at full brightness
    if (elapsedMinutes >= rampMinutes)
    {
        return SUNRISE_MAX_BRIGHTNESS;
    }

    // Calculate the brightness increase per minute.
    uint16_t brightnessStep = (SUNRISE_MAX_BRIGHTNESS - SUNRISE_START_BRIGHTNESS + rampMinutes - 1)/ rampMinutes;
    uint16_t brightness = SUNRISE_START_BRIGHTNESS + elapsedMinutes * brightnessStep;

    if (brightness > SUNRISE_MAX_BRIGHTNESS)
    {
        brightness = SUNRISE_MAX_BRIGHTNESS;
    }
    return (uint8_t)brightness;
}

bool alarm_sunrise_is_active()
{
    return sunriseActive;
}

static void alarm_check_new_day()
{
    uint8_t currentDay = rtc_get_day();

    if (currentDay == lastAlarmDay)
    {
        return;
    }

    lastAlarmDay = currentDay;

    for (uint8_t i = 0; i < NUMBER_OF_ALARMS; i++)
    {
        alarmHandledToday[i] = false;
        alarmRinging[i] = false;
    }

    Serial.println("New day: alarm states reset");
}

static AlarmState alarm_get_state(uint8_t alarmIndex)
{
    if (alarmIndex >= NUMBER_OF_ALARMS)
        return ALARM_INACTIVE;

    if (!alarm_is_scheduled_today(alarmIndex))
        return ALARM_INACTIVE;

    // This alarm occurrence has already been handled
    if (alarmHandledToday[alarmIndex])
        return ALARM_INACTIVE;

    uint16_t currentTime = rtc_get_hour() * 60 + rtc_get_minute();
    uint16_t alarmTime = alarms[alarmIndex].hour * 60 + alarms[alarmIndex].minute;
    uint8_t risingTime = alarms[alarmIndex].risingTime;
    int sunriseStartTime = (int)alarmTime - risingTime;

    if (currentTime < sunriseStartTime)
    {
        return ALARM_INACTIVE;
    }

    if (currentTime < alarmTime)
    {
        return ALARM_SUNRISE;
    }
    return ALARM_DUE;
}

static void alarm_start_ringing(uint8_t alarmIndex)
{
    if (alarmIndex >= NUMBER_OF_ALARMS)
        return;

    if (alarmHandledToday[alarmIndex])
        return;

    // Do not replace an alarm that is already ringing
    for (uint8_t i = 0; i < NUMBER_OF_ALARMS; i++)
    {
        if (alarmRinging[i])
        {
            return;
        }
    }

    alarmRinging[alarmIndex] = true;

    audio_play_sound(alarms[alarmIndex].sound);

    Serial.print("Alarm ");
    Serial.print(alarmIndex + 1);
    Serial.println(" is ringing");
}

static void alarm_validate(uint8_t alarmIndex)
{
    if (alarmIndex >= NUMBER_OF_ALARMS)
        return;

    alarmHandledToday[alarmIndex] = true;
    alarmRinging[alarmIndex] = false;

    audio_stop();

    Serial.print("Alarm ");
    Serial.print(alarmIndex + 1);
    Serial.println(" validated");
}

static void alarm_update_validation()
{
    if (!button_just_pressed(BUTTON_VALIDATE))
    {
        return;
    }

    // First priority: stop a ringing alarm
    for (uint8_t i = 0; i < NUMBER_OF_ALARMS; i++)
    {
        if (alarmRinging[i])
        {
            alarm_validate(i);
            return;
        }
    }

    // Otherwise, validate the earliest active sunrise
    int8_t selectedAlarm = -1;
    uint16_t selectedAlarmTime = 1440;

    for (uint8_t i = 0; i < NUMBER_OF_ALARMS; i++)
    {
        if (alarm_get_state(i) != ALARM_SUNRISE)
        {
            continue;
        }

        uint16_t alarmTime = alarms[i].hour * 60 + alarms[i].minute;

        if (alarmTime < selectedAlarmTime)
        {
            selectedAlarmTime = alarmTime;
            selectedAlarm = i;
        }
    }

    if (selectedAlarm >= 0)
    {
        alarm_validate(selectedAlarm);
    }
}