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
void screen_show_alarm_setting_page(uint8_t selectedAlarm);
void screen_show_alarm_time_setting_page(uint8_t hour,uint8_t minute);
void screen_update_alarm_setting_hour( uint8_t hour,bool show);
void screen_update_alarm_setting_minute( uint8_t minute,bool show);

// Alarm rising time setting
void screen_show_rising_time_setting_page(uint8_t risingTime);
void screen_update_rising_time_setting( uint8_t risingTime);

// Alarm sound setting
void screen_show_sound_setting_page(uint8_t sound);
void screen_update_sound_setting(uint8_t sound);

// Alarm days setting
void screen_show_days_setting_page(bool monday, bool tuesday, bool wednesday, bool thursday, bool friday, bool saturday, bool sunday);
void screen_update_days_setting(bool monday, bool tuesday, bool wednesday, bool thursday, bool friday, bool saturday, bool sunday);

// Alarm enabled setting
void screen_show_enabled_setting_page(bool enabled);
void screen_update_enabled_setting(bool enabled);

void screen_reset_alarm_page();

#endif