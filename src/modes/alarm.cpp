#include <Arduino.h>
#include "alarm.h"

#include "../hardware/button.h"
#include "../hardware/screen.h"

// Alarm configuration
static const uint8_t NUMBER_OF_ALARMS = 5;

// Alarm setting state
static uint8_t selectedAlarm = 0;
static bool selectionFinished = false;

// Initialization
void alarm_init()
{
    selectedAlarm = 0;
    selectionFinished = false;
}

// Start the alarm selection screen
void alarm_start_setting()
{
    selectedAlarm = 0;
    selectionFinished = false;

    screen_show_alarm_setting_page(selectedAlarm);
}

// Handle alarm selection buttons*
void alarm_update()
{
    // Move to the next alarm
    if (button_just_pressed(BUTTON_PLUS))
    {
        if (selectedAlarm == 0)
            selectedAlarm = NUMBER_OF_ALARMS - 1;
        else
            selectedAlarm--;

        screen_show_alarm_setting_page(selectedAlarm);
    }

    // Move to the previous alarm
    if (button_just_pressed(BUTTON_MINUS))
    {
        selectedAlarm++;

        if (selectedAlarm >= NUMBER_OF_ALARMS)
            selectedAlarm = 0;

        screen_show_alarm_setting_page(selectedAlarm);
    }

    // Validate the selected alarm
    if (button_just_pressed(BUTTON_VALIDATE))
    {
        selectionFinished = true;
    }
}

// Status
bool alarm_selection_finished()
{
    return selectionFinished;
}

uint8_t alarm_get_selected()
{
    return selectedAlarm;
}