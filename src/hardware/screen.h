
#ifndef SCREEN_H
#define SCREEN_H

#define ENABLE_GxEPD2_GFX 0

#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_4C.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>

void screen_init();

void screen_show_time(int hour, int minute);

void screen_show_alarm(uint8_t alarmNumber);

void screen_show_active_alarms(
    bool alarm1,
    bool alarm2,
    bool alarm3,
    bool alarm4,
    bool alarm5
);

void screen_show_home(
    int hour,
    int minute,
    bool alarm1,
    bool alarm2,
    bool alarm3,
    bool alarm4,
    bool alarm5
);

#endif