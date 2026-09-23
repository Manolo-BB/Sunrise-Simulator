#ifndef ALARM_H
#define ALARM_H

#include <Arduino.h>

// Alarm management
void alarm_init();
void alarm_start_setting();
void alarm_update();

// Alarm selection
uint8_t alarm_get_selected();
bool alarm_selection_finished();

#endif