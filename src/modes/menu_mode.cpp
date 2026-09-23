
#include <Arduino.h>

#include "menu_mode.h"
#include "modes/clock.h"
#include "modes/alarm.h"

#include "hardware/button.h"
#include "hardware/screen.h"

//This mode aim to know the state of the display in menu

MenuState menuState = MENU_INACTIVE;
ParamOption selectedOption = PARAM_TIME;

void menu_init()
{
    menuState = MENU_INACTIVE;
    selectedOption = PARAM_TIME;
}

//Change the menuState based on user interactions
void menu_update()
{
    if (menuState == MENU_INACTIVE)
    {
        if (button_just_pressed(BUTTON_PARAM))
        {
            menuState = PARAM_PAGE;
            selectedOption = PARAM_TIME;

            screen_show_param_page(selectedOption);
        }
        return;
    }

    if (menuState == PARAM_PAGE)
    {
        // PLUS
        if (button_just_pressed(BUTTON_PLUS))
        {
            selectedOption = PARAM_TIME;
            screen_show_param_page(selectedOption);
        }

        // MINUS
        if (button_just_pressed(BUTTON_MINUS))
        {
            selectedOption = PARAM_ALARM;
            screen_show_param_page(selectedOption);
        }

        // VALIDATE
        if (button_just_pressed(BUTTON_VALIDATE))
        {
            if(selectedOption == PARAM_TIME)
            {
                menuState = TIME_SETTING_PAGE;
                clock_start_time_setting();
                screen_show_time_setting_page();
                return;
            }
            else if(selectedOption == PARAM_ALARM)
            {
                menuState = ALARM_SETTING_PAGE;
                alarm_start_setting();
                return;
            }
        }

        //PARAM
        if (button_just_pressed(BUTTON_PARAM))
        {
            menuState = MENU_INACTIVE;
            return;
        }
        return;
    }

    //Time setting page
    if (menuState == TIME_SETTING_PAGE)
    {
        clock_time_setting_update();

        if (clock_time_setting_finished())
        {
            menuState = MENU_INACTIVE;
            return;
        }
        
        if (button_just_pressed(BUTTON_PARAM))
        {
            menuState = PARAM_PAGE;
            selectedOption = PARAM_TIME;

            screen_show_param_page(selectedOption);
            return;
        }
    }
    
    //Alarm setting page
    if (menuState == ALARM_SETTING_PAGE)
    {
        if (button_just_pressed(BUTTON_PARAM))
        {
            menuState = PARAM_PAGE;
            selectedOption = PARAM_ALARM;

            screen_show_param_page(selectedOption);
            return;
        }
        

        alarm_update();

        if (alarm_selection_finished())
        {
            menuState = ALARM_TIME_SETTING_PAGE;
            alarm_start_alarm_setting();
            return;
        }
        return;
    }

    if (menuState == ALARM_TIME_SETTING_PAGE)
    {
        if (button_just_pressed(BUTTON_PARAM))
        {
            menuState = ALARM_SETTING_PAGE;
            screen_reset_alarm_page();
            alarm_start_setting();
            return;
        }

        alarm_setting_update();

        if (alarm_setting_finished())
        {
            menuState = ALARM_SETTING_PAGE;
            screen_reset_alarm_page();
            alarm_start_setting();
            return;
        }

        return;
    }
}

bool menu_is_active()
{
    return menuState != MENU_INACTIVE;
}