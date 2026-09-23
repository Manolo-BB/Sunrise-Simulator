#ifndef SCREEN_H
#define SCREEN_H

// Display initialization
void screen_init();

// Home screen
void screen_show_home_complete( uint8_t hour, uint8_t minute, bool alarm1, bool alarm2, bool alarm3, bool alarm4, bool alarm5);
void screen_show_home(uint8_t hour, uint8_t minute);
void screen_update_home_date();

// Parameter screen
void screen_show_param_page(uint8_t selectedOption);

// Time setting screen
void screen_show_time_setting_page();
void screen_update_setting_hour( uint8_t hour, bool show);
void screen_update_setting_minute( uint8_t minute, bool show);

// Date setting screen
void screen_show_date_setting_page();
void screen_update_setting_date(uint8_t day, uint8_t month, uint16_t year, uint8_t field, bool show);

// Alarm setting screen
void screen_show_alarm_setting_page();

#endif