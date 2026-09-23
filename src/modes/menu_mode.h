#ifndef MENU_MODE_H
#define MENU_MODE_H

enum MenuState
{
    MENU_INACTIVE,
    PARAM_PAGE,
    TIME_SETTING_PAGE,
    ALARM_SETTING_PAGE,
    ALARM_TIME_SETTING_PAGE
};

enum ParamOption
{
    PARAM_TIME,
    PARAM_ALARM
};

void menu_init();
void menu_update();

bool menu_is_active();

#endif