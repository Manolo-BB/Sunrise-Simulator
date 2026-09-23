#ifndef ALARM_H
#define ALARM_H

#include <Arduino.h>

enum AlarmSettingStep
{
    ALARM_SET_HOUR,
    ALARM_SET_MINUTE,
    ALARM_SET_RISING_TIME,
    ALARM_SET_SONG,
    ALARM_SET_SCHEDULE
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
bool alarm_setting_finished();

uint8_t alarm_get_setting_hour();
uint8_t alarm_get_setting_minute();
uint8_t alarm_get_setting_rising_time();


#endif