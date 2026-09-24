#ifndef ALARM_H
#define ALARM_H

#include <Arduino.h>

#define VALIDATE_LONG_PRESS_TIME 1000
#define BLINK_INTERVAL 500


#define NUMBER_OF_ALARMS 5
// Maximum sunrise time in minutes
#define MAX_RISING_TIME 45
#define MIN_RISING_TIME 5

#define SUNRISE_MAX_BRIGHTNESS   255
#define SUNRISE_START_BRIGHTNESS 5
#define SUNRISE_FULL_LIGHT_TIME  5

enum AlarmSettingStep
{
    ALARM_SET_HOUR,
    ALARM_SET_MINUTE,
    ALARM_SET_RISING_TIME,
    ALARM_SET_SOUND,
    ALARM_SET_DAYS,
    ALARM_SET_ENABLED
};

enum AlarmState
{
    ALARM_INACTIVE,
    ALARM_SUNRISE,
    ALARM_DUE
};

// Alarm management
void alarm_init();
void alarm_start_setting();
void alarm_update();

// Alarm selection
uint8_t alarm_get_selected();
bool alarm_selection_finished();

// Alarm setting
void alarm_start_alarm_setting();
void alarm_setting_update();

// Check if the complete alarm configuration is finished
bool alarm_setting_finished();
// Get alarm state
bool alarm_is_enabled(uint8_t alarmIndex);
bool alarm_is_scheduled_today(uint8_t alarmIndex);
int8_t alarm_get_next_today();

//Get alarm configuration
uint8_t alarm_get_setting_hour();
uint8_t alarm_get_setting_minute();
uint8_t alarm_get_setting_rising_time();
uint8_t alarm_get_setting_sound();

//Sunrise functions
void alarm_sunrise_update();
bool alarm_sunrise_is_active();

#endif