#ifndef CLOCK_H
#define CLOCK_H

#include <Arduino.h>

// Time and date setting steps
enum ClockSettingStep
{
    CLOCK_SET_HOUR,
    CLOCK_SET_MINUTE,
    CLOCK_SET_DAY,
    CLOCK_SET_MONTH,
    CLOCK_SET_YEAR
};

// Clock management
void clock_init();
void clock_update();

// Time and date setting
void clock_start_time_setting();
void clock_time_setting_update();
bool clock_time_setting_finished();

// Current setting values
uint8_t clock_get_setting_hour();
uint8_t clock_get_setting_minute();
uint8_t clock_get_setting_day();
uint8_t clock_get_setting_month();
uint16_t clock_get_setting_year();

#endif