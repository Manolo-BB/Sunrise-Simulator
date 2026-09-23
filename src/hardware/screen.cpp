#include <Arduino.h>
#include <GxEPD2_BW.h>

//Fonts
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>

//Personal libraries
#include "config.h"
#include "screen.h"
#include "rtc.h"
#include "audio.h"

#define ENABLE_GxEPD2_GFX 0

//Display configuration
GxEPD2_BW<GxEPD2_290_BS, GxEPD2_290_BS::HEIGHT>
display(GxEPD2_290_BS(
            DISPLAY_CS,
            DISPLAY_DC,
            DISPLAY_RES,
            DISPLAY_BUSY)
);

// Home time refresh window
static const int16_t TIME_WINDOW_X = 55;
static const int16_t TIME_WINDOW_Y = 40;
static const uint16_t TIME_WINDOW_W = 266;
static const uint16_t TIME_WINDOW_H = 50;

// Home date refresh window
static const int16_t DATE_WINDOW_X = 140;
static const int16_t DATE_WINDOW_Y = 105;
static const uint16_t DATE_WINDOW_W = 150;
static const uint16_t DATE_WINDOW_H = 23;

// Time setting positions
static const int16_t SETTING_HOUR_X = 75;
static const int16_t SETTING_HOUR_Y = 95;
static const uint16_t SETTING_HOUR_W = 65;
static const uint16_t SETTING_HOUR_H = 40;

static const int16_t SETTING_MINUTE_X = 155;
static const int16_t SETTING_MINUTE_Y = 95;
static const uint16_t SETTING_MINUTE_W = 65;
static const uint16_t SETTING_MINUTE_H = 40;

static const int16_t SETTING_SEPARATOR_X = 135;
static const int16_t SETTING_SEPARATOR_Y = 95;

// Date setting positions
static const int16_t SETTING_DATE_X = 35;
static const int16_t SETTING_DATE_Y = 95;
static const uint16_t SETTING_DATE_W = 230;
static const uint16_t SETTING_DATE_H = 40;

// Stat of alarm screen
static bool alarmPageDisplayed = false;
static bool soundPageDisplayed = false;

// ========================================
// Internal drawing functions
// ========================================

// Draw the current time centered on the display
static void draw_time(uint8_t hour, uint8_t minute)
{
    char timeText[6];

    //Make a string with the hour
    snprintf( timeText, sizeof(timeText),"%02d:%02d", hour, minute);
    display.setFont(&FreeMonoBold24pt7b);
    display.setTextColor(GxEPD_BLACK);

    int16_t textX;
    int16_t textY;
    uint16_t textW;
    uint16_t textH;

    //Get the size of the string and center it on the display
    display.getTextBounds(timeText, 0, 0, &textX, &textY, &textW,&textH);

    int16_t x = (display.width() - textW) / 2 - textX;
    int16_t y = (display.height() - textH) / 2 - textY;

    display.setCursor(x, y);
    display.print(timeText);
}

// Draw the current date
static void draw_date(uint8_t day, uint8_t month, uint16_t year)
{
    char dateText[11];

    snprintf(dateText, sizeof(dateText),"%02d/%02d/%04d",day,month,year);
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(GxEPD_BLACK);

    int16_t textX;
    int16_t textY;
    uint16_t textW;
    uint16_t textH;

    display.getTextBounds(dateText, 0, 0, &textX, &textY, &textW,&textH);
    display.setCursor(display.width() - textW - 5,display.height() - 5);
    display.print(dateText);
}

// Draw the date during date setting
static void draw_setting_date(uint8_t day, uint8_t month,uint16_t year,uint8_t field, bool show)
{
    display.setFont(&FreeMonoBold12pt7b);
    display.setTextColor(GxEPD_BLACK);

    char dayText[3];
    char monthText[3];
    char yearText[5];

    snprintf(dayText, sizeof(dayText), "%02d", day);
    snprintf(monthText, sizeof(monthText), "%02d", month);
    snprintf(yearText, sizeof(yearText), "%04d", year);

    int16_t textX;
    int16_t textY;
    uint16_t dayW;
    uint16_t dayH;
    uint16_t monthW;
    uint16_t monthH;
    uint16_t yearW;
    uint16_t yearH;
    uint16_t separatorW;
    uint16_t separatorH;

    display.getTextBounds(dayText, 0, 0, &textX,&textY, &dayW,&dayH);
    display.getTextBounds(monthText,0,0,&textX,&textY,&monthW,&monthH);
    display.getTextBounds( yearText,0, 0,&textX, &textY, &yearW,&yearH);
    display.getTextBounds( "/",0, 0,&textX,&textY, &separatorW, &separatorH );

    uint16_t totalWidth = dayW + separatorW + monthW +separatorW + yearW;
    int16_t startX = (display.width() - totalWidth) / 2;

    // Day
    if (field != 0 || show)
    {
        display.setCursor( startX, SETTING_DATE_Y);
        display.print(dayText);
    }
    startX += dayW;

    // First separator
    display.setCursor( startX, SETTING_DATE_Y);
    display.print("/");
    startX += separatorW;

    // Month
    if (field != 1 || show)
    {
        display.setCursor( startX, SETTING_DATE_Y);
        display.print(monthText);
    }
    startX += monthW;

    // Second separator
    display.setCursor(startX,SETTING_DATE_Y );
    display.print("/");
    startX += separatorW;

    // Year
    if (field != 2 || show)
    {
        display.setCursor(startX, SETTING_DATE_Y);
        display.print(yearText);
    }
}

// Draw one alarm icon
// This function was entirely maid by an LLM
static void draw_alarm_icon(int16_t x, int16_t y, uint8_t alarmNumber)
{
    // Alarm body
    display.fillCircle(x + 14,y + 14, 11, GxEPD_BLACK);

    // Left foot
    display.fillRect( x + 5, y + 23, 6, 3, GxEPD_BLACK);

    display.drawLine( x + 7, y + 24, x + 4,y + 28, GxEPD_BLACK);

    // Right foot
    display.fillRect( x + 17, y + 23, 6, 3,GxEPD_BLACK);
    display.drawLine( x + 21, y + 24, x + 24, y + 28, GxEPD_BLACK);

    // Left bell
    display.fillTriangle( x + 4, y + 7, x + 0, y + 2, x + 7, y + 4, GxEPD_BLACK);

    // Right bell
    display.fillTriangle( x + 24, y + 7, x + 28, y + 2, x + 21, y + 4, GxEPD_BLACK);

    // Alarm number
    char numberText[3];

    snprintf(numberText, sizeof(numberText),"%d", alarmNumber);
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(GxEPD_WHITE);

    int16_t textX;
    int16_t textY;
    uint16_t textW;
    uint16_t textH;

    display.getTextBounds( numberText,0,0,&textX,&textY, &textW,&textH);
    display.setCursor( x + 14 - textW / 2 - textX, y + 14 - textH / 2 - textY);
    display.print(numberText);
}

// Draw all active alarm icons
static void draw_active_alarms( bool alarm1,bool alarm2, bool alarm3, bool alarm4, bool alarm5)
{
    const int16_t ALARM_START_X = 5;
    const int16_t ALARM_Y = 5;
    const int16_t ALARM_SPACING = 35;

    int16_t x = ALARM_START_X;

    if (alarm1)
    {
        draw_alarm_icon(x, ALARM_Y, 1);
        x += ALARM_SPACING;
    }

    if (alarm2)
    {
        draw_alarm_icon(x, ALARM_Y, 2);
        x += ALARM_SPACING;
    }

    if (alarm3)
    {
        draw_alarm_icon(x, ALARM_Y, 3);
        x += ALARM_SPACING;
    }

    if (alarm4)
    {
        draw_alarm_icon(x, ALARM_Y, 4);
        x += ALARM_SPACING;
    }

    if (alarm5)
    {
        draw_alarm_icon(x, ALARM_Y, 5);
    }
}

// ===================================
// Initialization
// ===================================
void screen_init()
{
    display.init(115200, true, 50, false);
    display.setRotation(3);
    display.setPartialWindow(  0, 0, display.width(), display.height());;

    // Draws the display page by page to update the entire screen
    display.firstPage();
    do
    {
        display.fillScreen(GxEPD_WHITE);
    }
    while (display.nextPage());
    //We hibernate to switch the ESP into low power
    display.hibernate();
}

void screen_show_home_complete( uint8_t hour, uint8_t minute, bool alarm1, bool alarm2, bool alarm3, bool alarm4, bool alarm5)
{
    display.setPartialWindow(  0, 0, display.width(), display.height());;
    display.firstPage();
    do
    {
        display.fillScreen(GxEPD_WHITE);

        // Time is depicted in its own dedicated space
        draw_time(hour, minute);

        // Same for alarms and date
        draw_active_alarms( alarm1, alarm2, alarm3, alarm4, alarm5);

        // Date
        draw_date( rtc_get_day(), rtc_get_month(), rtc_get_year());
    }
    while (display.nextPage());
    display.hibernate();
}

// Update only the time on the home screen
void screen_show_home(uint8_t hour, uint8_t minute)
{
    display.setPartialWindow( TIME_WINDOW_X, TIME_WINDOW_Y, TIME_WINDOW_W, TIME_WINDOW_H);
    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);
        draw_time(hour, minute);

    } while (display.nextPage());

    display.hibernate();
}

// Update only the date on the home screen
void screen_update_home_date()
{
    uint8_t day = rtc_get_day();
    uint8_t month = rtc_get_month();
    uint16_t year = rtc_get_year();

    display.setPartialWindow( DATE_WINDOW_X, DATE_WINDOW_Y, DATE_WINDOW_W, DATE_WINDOW_H);
    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);
        draw_date( day, month, year);
    }
    while (display.nextPage());

    display.hibernate();
}

// Parameter screen
void screen_show_param_page(uint8_t selectedOption)
{
    display.setPartialWindow( 0,0 , display.width(), display.height());
    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);

        // Title
        display.setTextColor(GxEPD_BLACK);
        display.setFont(&FreeMonoBold24pt7b);
        display.setCursor(15, 30);
        display.print("PARAMETERS");

        // Time setting option
        const int16_t TIME_BOX_X = 20;
        const int16_t TIME_BOX_Y = 45;
        const uint16_t TIME_BOX_W = 256;
        const uint16_t TIME_BOX_H = 30;

        // Alarm setting option
        const int16_t ALARM_BOX_X = 20;
        const int16_t ALARM_BOX_Y = 82;
        const uint16_t ALARM_BOX_W = 256;
        const uint16_t ALARM_BOX_H = 30;

        // Time option
        if (selectedOption == 0)
        {
            display.fillRect( TIME_BOX_X, TIME_BOX_Y, TIME_BOX_W, TIME_BOX_H, GxEPD_BLACK);
            display.setTextColor(GxEPD_WHITE);
        }
        else
        {
            display.drawRect( TIME_BOX_X, TIME_BOX_Y, TIME_BOX_W, TIME_BOX_H,GxEPD_BLACK);
            display.setTextColor(GxEPD_BLACK);
        }

        display.setFont(&FreeMonoBold9pt7b);
        display.setCursor(35, 66);
        display.print("Reglage Heure");

        // Alarm option
        if (selectedOption == 1)
        {
            display.fillRect( ALARM_BOX_X, ALARM_BOX_Y, ALARM_BOX_W, ALARM_BOX_H, GxEPD_BLACK);
            display.setTextColor(GxEPD_WHITE);
        }
        else
        {
            display.drawRect( ALARM_BOX_X, ALARM_BOX_Y, ALARM_BOX_W, ALARM_BOX_H, GxEPD_BLACK);
            display.setTextColor(GxEPD_BLACK);
        }

        display.setCursor(35, 103);
        display.print("Reglage Alarme");
    }
    while (display.nextPage());

    display.hibernate();
}

// Time setting screen
void screen_show_time_setting_page()
{
    uint8_t hour = rtc_get_hour();
    uint8_t minute = rtc_get_minute();

    display.setPartialWindow(  0, 0, display.width(), display.height());;
    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);

        // Title
        display.setFont(&FreeMonoBold9pt7b);
        display.setTextColor(GxEPD_BLACK);
        display.setCursor(85, 30);
        display.print("REGLAGE HEURE");

        // Hour
        display.setFont(&FreeMonoBold24pt7b);
        display.setCursor( SETTING_HOUR_X, SETTING_HOUR_Y );

        if (hour < 10)
            display.print("0");

        display.print(hour);

        // Separator
        display.setCursor(SETTING_SEPARATOR_X, SETTING_SEPARATOR_Y);
        display.print(":");

        // Minute
        display.setCursor(SETTING_MINUTE_X, SETTING_MINUTE_Y);

        if (minute < 10)
            display.print("0");

        display.print(minute);
    }
    while (display.nextPage());

    display.hibernate();
}

// Update hour during time setting
void screen_update_setting_hour( uint8_t hour, bool show)
{
    display.setPartialWindow(SETTING_HOUR_X, SETTING_HOUR_Y - SETTING_HOUR_H, SETTING_HOUR_W, SETTING_HOUR_H);
    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);

        if (show)
        {
            display.setFont(&FreeMonoBold24pt7b);
            display.setTextColor(GxEPD_BLACK);
            display.setCursor( SETTING_HOUR_X, SETTING_HOUR_Y );

            if (hour < 10)
                display.print("0");

            display.print(hour);
        }
    }
    while (display.nextPage());

    display.hibernate();
}

// Update minute during time setting
void screen_update_setting_minute( uint8_t minute, bool show)
{
    display.setPartialWindow( SETTING_MINUTE_X, SETTING_MINUTE_Y - SETTING_MINUTE_H,SETTING_MINUTE_W,SETTING_MINUTE_H);
    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);

        if (show)
        {
            display.setFont(&FreeMonoBold24pt7b);
            display.setTextColor(GxEPD_BLACK);
            display.setCursor( SETTING_MINUTE_X, SETTING_MINUTE_Y );

            if (minute < 10)
                display.print("0");

            display.print(minute);
        }
    }
    while (display.nextPage());

    display.hibernate();
}

// Date setting screen
void screen_show_date_setting_page()
{
    uint8_t day = rtc_get_day();
    uint8_t month = rtc_get_month();
    uint16_t year = rtc_get_year();

    display.setPartialWindow(  0, 0, display.width(), display.height());;
    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);

        display.setFont(&FreeMonoBold9pt7b);
        display.setTextColor(GxEPD_BLACK);
        display.setCursor(85, 30);
        display.print("REGLAGE DATE");
        
        display.setFont(&FreeMonoBold12pt7b);

        char dateText[11];

        snprintf( dateText, sizeof(dateText),"%02d/%02d/%04d", day, month, year);

        int16_t textX;
        int16_t textY;
        uint16_t textW;
        uint16_t textH;

        display.getTextBounds( dateText, 0, 0, &textX, &textY,&textW,&textH);
        display.setCursor( (display.width() - textW) / 2 - textX, SETTING_DATE_Y);
        display.print(dateText);
    }
    while (display.nextPage());

    display.hibernate();
}

// Update date during date setting
void screen_update_setting_date(uint8_t day, uint8_t month, uint16_t year, uint8_t field, bool show)
{
    display.setPartialWindow( SETTING_DATE_X, SETTING_DATE_Y - SETTING_DATE_H,SETTING_DATE_W, SETTING_DATE_H);
    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);
        draw_setting_date( day,month,year,field,show);
    } while (display.nextPage());

    display.hibernate();
}

// Alarm setting screen
void screen_show_alarm_setting_page(uint8_t selectedAlarm)
{
    const int16_t ALARM_WINDOW_X = 65;
    const int16_t ALARM_WINDOW_Y = 35;
    const uint16_t ALARM_WINDOW_W = 160;
    const uint16_t ALARM_WINDOW_H = 80;

    if (!alarmPageDisplayed)
    {
        display.setPartialWindow(  0, 0, display.width(), display.height());;
        display.firstPage();

        do
        {
            display.fillScreen(GxEPD_WHITE);
            display.setTextColor(GxEPD_BLACK);
            // Title
            display.setFont(&FreeMonoBold12pt7b);
            display.setCursor(60, 20);
            display.print("REGLAGE ALARME");

            // Alarm list
            display.setFont(&FreeMonoBold9pt7b);

            for (uint8_t i = 0; i < 5; i++)
            {
                uint16_t y = 40 + i * 15;
                display.setCursor(75, y);
                if (i == selectedAlarm)
                    display.print("> ");
                else
                    display.print("  ");

                display.print("Alarme ");
                display.print(i + 1);
            }

        } while (display.nextPage());

        alarmPageDisplayed = true;
        display.hibernate();

        return;
    }

    // Only update the alarm selection
    display.setPartialWindow( ALARM_WINDOW_X, ALARM_WINDOW_Y, ALARM_WINDOW_W, ALARM_WINDOW_H);

    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);
        display.setFont(&FreeMonoBold9pt7b);
        display.setTextColor(GxEPD_BLACK);

        for (uint8_t i = 0; i < 5; i++)
        {
            uint16_t y = 40 + i * 15;
            display.setCursor(75, y);

            if (i == selectedAlarm)
                display.print("> ");
            else
                display.print("  ");

            display.print("Alarme ");
            display.print(i + 1);
        }

    } while (display.nextPage());
    display.hibernate();
}

void screen_show_alarm_time_setting_page(uint8_t hour, uint8_t minute)
{
    display.setPartialWindow(  0, 0, display.width(), display.height());;
    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);

        display.setFont(&FreeMonoBold9pt7b);
        display.setTextColor(GxEPD_BLACK);
        display.setCursor(85, 30);
        display.print("ALARME - HEURE");

        display.setFont(&FreeMonoBold24pt7b);

        display.setCursor(SETTING_HOUR_X, SETTING_HOUR_Y);

        if (hour < 10)
            display.print("0");

        display.print(hour);
        display.setCursor(SETTING_SEPARATOR_X, SETTING_SEPARATOR_Y);
        display.print(":");
        display.setCursor(SETTING_MINUTE_X, SETTING_MINUTE_Y);

        if (minute < 10)
            display.print("0");

        display.print(minute);

    }
    while (display.nextPage());

    display.hibernate();
}

void screen_update_alarm_setting_hour(uint8_t hour, bool show)
{
    display.setPartialWindow(SETTING_HOUR_X, SETTING_HOUR_Y - SETTING_HOUR_H, SETTING_HOUR_W, SETTING_HOUR_H);
    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);

        if (show)
        {
            display.setFont(&FreeMonoBold24pt7b);
            display.setTextColor(GxEPD_BLACK);
            display.setCursor(SETTING_HOUR_X,SETTING_HOUR_Y );

            if (hour < 10)
                display.print("0");

            display.print(hour);
        }

    }
    while (display.nextPage());

    display.hibernate();
}

void screen_update_alarm_setting_minute(uint8_t minute, bool show)
{
    display.setPartialWindow(SETTING_MINUTE_X,SETTING_MINUTE_Y - SETTING_MINUTE_H,SETTING_MINUTE_W,SETTING_MINUTE_H);
    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);

        if (show)
        {
            display.setFont(&FreeMonoBold24pt7b);
            display.setTextColor(GxEPD_BLACK);
            display.setCursor( SETTING_MINUTE_X, SETTING_MINUTE_Y);

            if (minute < 10)
                display.print("0");

            display.print(minute);
        }

    }
    while (display.nextPage());

    display.hibernate();
}

void screen_show_rising_time_setting_page(uint8_t risingTime)
{
    display.setPartialWindow(  0, 0, display.width(), display.height());;
    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);

        display.setFont(&FreeMonoBold9pt7b);
        display.setTextColor(GxEPD_BLACK);
        display.setCursor(95, 30);
        display.print("RISING TIME");

        display.setFont(&FreeMonoBold24pt7b);
        display.setCursor(90, 95);

        if (risingTime < 10)
            display.print("0");

        display.print(risingTime);
        display.print(" min");

    }
    while (display.nextPage());

    display.hibernate();
}

void screen_update_rising_time_setting(uint8_t risingTime)
{
    const int16_t X = 70;
    const int16_t Y = 55;
    const uint16_t W = 190;
    const uint16_t H = 50;

    display.setPartialWindow(X, Y, W, H);
    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);
        display.setFont(&FreeMonoBold24pt7b);
        display.setTextColor(GxEPD_BLACK); 
        display.setCursor(90, 95);
           
        if (risingTime < 10)
            display.print("0");
                 
        display.print(risingTime);
        display.print(" min"); 
    }
    while (display.nextPage());

    display.hibernate();
}

// =========================================================
// SOUND SETTING
// =========================================================

void screen_show_sound_setting_page(uint8_t sound)
{
    display.setFullWindow();
    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);
        display.setTextColor(GxEPD_BLACK);

        display.setFont(&FreeMonoBold12pt7b);
        display.setCursor(70, 35);
        display.print("REGLAGE SON");

        display.setFont(&FreeMonoBold24pt7b);
        display.setCursor(125, 100);

        display.print(sound);

    }
    while (display.nextPage());

    display.hibernate();
}


void screen_update_sound_setting(uint8_t sound)
{
    const int16_t X = 90;
    const int16_t Y = 45;
    const uint16_t W = 120;
    const uint16_t H = 70;

    display.setPartialWindow(X, Y, W, H);
    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);
        display.setTextColor(GxEPD_BLACK);

        display.setFont(&FreeMonoBold24pt7b);
        display.setCursor(125, 100);

        display.print(sound);

    }
    while (display.nextPage());

    display.hibernate();
}


// =========================================================
// DAYS SETTING
// =========================================================

void screen_show_days_setting_page(
    bool monday,
    bool tuesday,
    bool wednesday,
    bool thursday,
    bool friday,
    bool saturday,
    bool sunday
)
{
    display.setFullWindow();
    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);
        display.setTextColor(GxEPD_BLACK);

        display.setFont(&FreeMonoBold12pt7b);
        display.setCursor(75, 30);
        display.print("JOURS");

        display.setFont(&FreeMonoBold9pt7b);

        display.setCursor(70, 55);
        display.print("LUN ");
        display.print(monday ? "X" : "-");

        display.setCursor(150, 55);
        display.print("MAR ");
        display.print(tuesday ? "X" : "-");

        display.setCursor(70, 75);
        display.print("MER ");
        display.print(wednesday ? "X" : "-");

        display.setCursor(150, 75);
        display.print("JEU ");
        display.print(thursday ? "X" : "-");

        display.setCursor(70, 95);
        display.print("VEN ");
        display.print(friday ? "X" : "-");

        display.setCursor(150, 95);
        display.print("SAM ");
        display.print(saturday ? "X" : "-");

        display.setCursor(70, 115);
        display.print("DIM ");
        display.print(sunday ? "X" : "-");

    }
    while (display.nextPage());

    display.hibernate();
}


void screen_update_days_setting(
    bool monday,
    bool tuesday,
    bool wednesday,
    bool thursday,
    bool friday,
    bool saturday,
    bool sunday
)
{
    screen_show_days_setting_page(
        monday,
        tuesday,
        wednesday,
        thursday,
        friday,
        saturday,
        sunday
    );
}


// =========================================================
// ENABLED SETTING
// =========================================================

void screen_show_enabled_setting_page(bool enabled)
{
    display.setFullWindow();
    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);
        display.setTextColor(GxEPD_BLACK);

        display.setFont(&FreeMonoBold12pt7b);
        display.setCursor(60, 35);
        display.print("REVEIL");

        display.setFont(&FreeMonoBold24pt7b);
        display.setCursor(85, 100);

        if (enabled)
            display.print("ON");
        else
            display.print("OFF");

    }
    while (display.nextPage());

    display.hibernate();
}


void screen_update_enabled_setting(bool enabled)
{
    const int16_t X = 55;
    const int16_t Y = 45;
    const uint16_t W = 180;
    const uint16_t H = 70;

    display.setPartialWindow(X, Y, W, H);
    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);
        display.setTextColor(GxEPD_BLACK);

        display.setFont(&FreeMonoBold24pt7b);
        display.setCursor(85, 100);

        if (enabled)
            display.print("ON");
        else
            display.print("OFF");

    }
    while (display.nextPage());

    display.hibernate();
}

void screen_reset_alarm_page()
{
    alarmPageDisplayed = false;
}

void screen_reset_sound_page()
{
    soundPageDisplayed = false; 
}