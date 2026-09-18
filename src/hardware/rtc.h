#ifndef RTC_H
#define RTC_H

#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include "config.h"

#define RTC_SYNC_INTERVAL 36000000UL

void rtc_init();
void rtc_update();
void rtc_set_time(uint8_t hour, uint8_t minute);
uint8_t rtc_get_hour();
uint8_t rtc_get_minute();

#endif