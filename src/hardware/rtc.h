#ifndef RTC_H
#define RTC_H

void rtc_init();
void rtc_update();

//Setters
void rtc_set_time(uint8_t hour, uint8_t minute);
void rtc_set_date(uint8_t day, uint8_t month, uint16_t year);

//Getters
uint8_t rtc_get_hour();
uint8_t rtc_get_minute();
uint8_t rtc_get_day();
uint8_t rtc_get_month();
uint16_t rtc_get_year();
uint8_t rtc_get_weekday();

#endif