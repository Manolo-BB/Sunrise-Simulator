#ifndef CLOCK_H
#define CLOCK_H

#include <Arduino.h>
#include "hardware/rtc.h"
#include "hardware/button.h"
#include "hardware/screen.h"

enum ClockSettingStep
{
    CLOCK_SET_HOUR,
    CLOCK_SET_MINUTE,
    CLOCK_SET_DATE
};

#define BLINK_INTERVAL 500

void clock_init();
void clock_update();
void clock_start_time_setting();
void clock_time_setting_update();
bool clock_time_setting_finished();
uint8_t clock_get_setting_hour();
uint8_t clock_get_setting_minute();
uint8_t clock_get_setting_day();
uint8_t clock_get_setting_month();
uint16_t clock_get_setting_year();
bool clock_is_blink_on();
bool clock_setting_hour_active();
bool clock_setting_minute_active();

#endif