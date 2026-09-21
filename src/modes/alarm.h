#ifndef ALARM_H
#define ALARM_H

#define MAX_ALARMS 5

void alarm_init();
void alarm_update();

void alarm_set(uint8_t id, uint8_t hour, uint8_t minute);
void alarm_enable(uint8_t id);
void alarm_disable(uint8_t id);

bool alarm_is_enabled(uint8_t id);
bool alarm_is_triggered(uint8_t id);

uint8_t alarm_get_hour(uint8_t id);
uint8_t alarm_get_minute(uint8_t id);

#endif