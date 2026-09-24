#ifndef ALARM_H
#define ALARM_H

#include <Arduino.h>

#define NUMBER_OF_ALARMS 5
// Maximum sunrise time in minutes
#define MAX_RISING_TIME 45

enum AlarmSettingStep
{
    ALARM_SET_HOUR,
    ALARM_SET_MINUTE,
    ALARM_SET_RISING_TIME,
    ALARM_SET_SOUND,
    ALARM_SET_DAYS,
    ALARM_SET_ENABLED
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

#endif