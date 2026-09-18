#include "menu_mode.h"

MenuState menuState = MENU_INACTIVE;
ParamOption selectedOption = PARAM_TIME;

void menu_init()
{
    menuState = MENU_INACTIVE;
    selectedOption = PARAM_TIME;
}

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
            else
            {
                menuState = ALARM_SETTING_PAGE;
                screen_show_alarm_setting_page();
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
            selectedOption = PARAM_TIME;

            screen_show_param_page(selectedOption);
        }
        return;
    }
}

bool menu_is_active()
{
    return menuState != MENU_INACTIVE;
}