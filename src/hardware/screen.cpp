#include "screen.h"
#include "config.h"

GxEPD2_BW<GxEPD2_290_BS, GxEPD2_290_BS::HEIGHT>
display(
    GxEPD2_290_BS(
        DISPLAY_CS,
        DISPLAY_DC,
        DISPLAY_RES,
        DISPLAY_BUSY
    )
);

// Home time refresh window
const int16_t timeWindowX = 55;
const int16_t timeWindowY = 40;
const uint16_t timeWindowW = 266;
const uint16_t timeWindowH = 80;

// Time setting positions and refresh windows
const int16_t settingHourX = 75;
const int16_t settingHourY = 95;
const uint16_t settingHourW = 65;
const uint16_t settingHourH = 40;

const int16_t settingMinuteX = 155;
const int16_t settingMinuteY = 95;
const uint16_t settingMinuteW = 65;
const uint16_t settingMinuteH = 40;

const int16_t settingSeparatorX = 135;
const int16_t settingSeparatorY = 95;

// Initialisation
void screen_init()
{
    display.init(115200, true, 50, false);
    display.setRotation(3);
    display.setFullWindow();
    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);
    }
    while (display.nextPage());
    display.hibernate();
}

//Hour and minute display
void screen_show_time(int hour, int minute)
{
    display.setRotation(3);
    display.setFullWindow();
    display.setFont(&FreeMonoBold24pt7b);
    display.setTextColor(GxEPD_BLACK);

    char timeText[6];
    snprintf(timeText, sizeof(timeText), "%02d:%02d", hour, minute);

    int16_t tbx;
    int16_t tby;
    uint16_t tbw;
    uint16_t tbh;

    display.getTextBounds(timeText, 0, 0, &tbx, &tby, &tbw, &tbh);

    int16_t x = (display.width() - tbw) / 2 - tbx;
    int16_t y = (display.height() - tbh) / 2 - tby;

    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);
        display.setCursor(x, y);
        display.print(timeText);
    }
    while (display.nextPage());

    display.hibernate();
}

//Add a function to draw an alarm icon with a number in the center
//This function was maid by an LLM
static void draw_alarm_icon(int16_t x, int16_t y, uint8_t alarmNumber)
{
    // Body
    display.fillCircle(
        x + 14,
        y + 14,
        11,
        GxEPD_BLACK
    );

    // Left foot
    display.fillRect(
        x + 5,
        y + 23,
        6,
        3,
        GxEPD_BLACK
    );

    display.drawLine(
        x + 7,
        y + 24,
        x + 4,
        y + 28,
        GxEPD_BLACK
    );

    // Right foot
    display.fillRect(
        x + 17,
        y + 23,
        6,
        3,
        GxEPD_BLACK
    );

    display.drawLine(
        x + 21,
        y + 24,
        x + 24,
        y + 28,
        GxEPD_BLACK
    );

    // Left bell
    display.fillTriangle(
        x + 4,
        y + 7,
        x + 0,
        y + 2,
        x + 7,
        y + 4,
        GxEPD_BLACK
    );

    // Right bell
    display.fillTriangle(
        x + 24,
        y + 7,
        x + 28,
        y + 2,
        x + 21,
        y + 4,
        GxEPD_BLACK
    );

    // Draw the alarm number in the center of the icon
    char numberText[3];

    snprintf(numberText, sizeof(numberText), "%d", alarmNumber);

    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(GxEPD_WHITE);

    int16_t tbx;
    int16_t tby;
    uint16_t tbw;
    uint16_t tbh;

    display.getTextBounds(numberText, 0, 0, &tbx, &tby, &tbw, &tbh);

    int16_t textX = x + 14 - tbw / 2 - tbx;
    int16_t textY = y + 14 - tbh / 2 - tby;

    display.setCursor(textX, textY);
    display.print(numberText);
}


// Display a single alarm icon with a number in the center
void screen_show_alarm(uint8_t alarmNumber)
{
    display.setRotation(3);
    display.setFullWindow();
    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);
        draw_alarm_icon(5, 5, alarmNumber);
    }
    while (display.nextPage());
    display.hibernate();
}

// Display active alarms
void screen_show_active_alarms(bool alarm1, bool alarm2, bool alarm3, bool alarm4, bool alarm5)
{
    display.setRotation(3);
    display.setFullWindow();
    display.firstPage();
    do
    {
        display.fillScreen(GxEPD_WHITE);
        int16_t x = 5;
        if (alarm1)
        {
            draw_alarm_icon(x, 5, 1);
            x += 35;
        }
        if (alarm2)
        {
            draw_alarm_icon(x, 5, 2);
            x += 35;
        }
        if (alarm3)
        {
            draw_alarm_icon(x, 5, 3);
            x += 35;
        }
        if (alarm4)
        {
            draw_alarm_icon(x, 5, 4);
            x += 35;
        }
        if (alarm5)
        {
            draw_alarm_icon(x, 5, 5);
        }
    }
    while (display.nextPage());
    display.hibernate();
}

// Display the hour at the center of the screen
void draw_time(int hour, int minute)
{
    char timeText[6];
    snprintf(timeText, sizeof(timeText), "%02d:%02d", hour, minute);

    display.setFont(&FreeMonoBold24pt7b);
    display.setTextColor(GxEPD_BLACK);

    int16_t tbx;
    int16_t tby;
    uint16_t tbw;
    uint16_t tbh;

    display.getTextBounds(timeText, 0, 0, &tbx, &tby, &tbw, &tbh);

    int16_t timeX = (display.width() - tbw) / 2 - tbx;
    int16_t timeY = (display.height() - tbh) / 2 - tby;

    display.setCursor(timeX, timeY);
    display.print(timeText);
}

void screen_show_home_complete(int hour, int minute, bool alarm1, bool alarm2, bool alarm3, bool alarm4, bool alarm5)
{
    display.setRotation(3);
    display.setFullWindow();
    display.firstPage();
    do
    {
        display.fillScreen(GxEPD_WHITE);

        draw_time(hour, minute);

        int16_t alarmX = 5;
        const int16_t alarmSpacing = 35;
        if (alarm1)
        {
            draw_alarm_icon(alarmX, 5, 1);
            alarmX += alarmSpacing;
        }
        if (alarm2)
        {
            draw_alarm_icon(alarmX, 5, 2);
            alarmX += alarmSpacing;
        }
        if (alarm3)
        {
            draw_alarm_icon(alarmX, 5, 3);
            alarmX += alarmSpacing;
        }
        if (alarm4)
        {
            draw_alarm_icon(alarmX, 5, 4);
            alarmX += alarmSpacing;
        }
        if (alarm5)
        {
            draw_alarm_icon(alarmX, 5, 5);
        }
    }
    while (display.nextPage());
    display.hibernate();
}

void screen_show_home(int hour, int minute)
{
    display.setRotation(3);
    display.setFont(&FreeMonoBold24pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.setPartialWindow(
        timeWindowX,
        timeWindowY,
        timeWindowW,
        timeWindowH
    );

    display.firstPage();

    do
    {
        display.fillRect(
            timeWindowX,
            timeWindowY,
            timeWindowW,
            timeWindowH,
            GxEPD_WHITE
        );

        draw_time(hour, minute);
    }
    while (display.nextPage());

    display.hibernate();
}


void screen_show_param_page(uint8_t selectedOption)
{
    display.setRotation(3);
    display.setFullWindow();
    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);

        display.setTextColor(GxEPD_BLACK);
        display.setFont(&FreeMonoBold24pt7b);
        display.setCursor(15, 30);
        display.print("PARAMETERS");

        //Time setting option
        int16_t timeBoxX = 20;
        int16_t timeBoxY = 45;
        uint16_t timeBoxW = 256;
        uint16_t timeBoxH = 30;

        //Alarm setting option
        int16_t alarmBoxX = 20;
        int16_t alarmBoxY = 82;
        uint16_t alarmBoxW = 256;
        uint16_t alarmBoxH = 30;

        if (selectedOption == 0)
        {
            display.fillRect(
                timeBoxX,
                timeBoxY,
                timeBoxW,
                timeBoxH,
                GxEPD_BLACK
            );

            display.setTextColor(GxEPD_WHITE);
        }
        else
        {
            display.drawRect(
                timeBoxX,
                timeBoxY,
                timeBoxW,
                timeBoxH,
                GxEPD_BLACK
            );

            display.setTextColor(GxEPD_BLACK);
        }

        display.setFont(&FreeMonoBold9pt7b);
        display.setCursor(35, 66);
        display.print("Reglage Heure");

        if (selectedOption == 1)
        {
            display.fillRect(
                alarmBoxX,
                alarmBoxY,
                alarmBoxW,
                alarmBoxH,
                GxEPD_BLACK
            );

            display.setTextColor(GxEPD_WHITE);
        }
        else
        {
            display.drawRect(
                alarmBoxX,
                alarmBoxY,
                alarmBoxW,
                alarmBoxH,
                GxEPD_BLACK
            );

            display.setTextColor(GxEPD_BLACK);
        }

        display.setCursor(35, 103);
        display.print("Reglage Alarme");
    }
    while (display.nextPage());

    display.hibernate();
}


void screen_show_time_setting_page()
{
    uint8_t hour = rtc_get_hour();
    uint8_t minute = rtc_get_minute();

    display.setFullWindow();
    display.firstPage();

    do
    {
        // Fond blanc
        display.fillScreen(GxEPD_WHITE);

        // Titre
        display.setFont(&FreeMonoBold9pt7b);
        display.setTextColor(GxEPD_BLACK);

        display.setCursor(85, 30);
        display.print("REGLAGE HEURE");

        // Heure
        display.setFont(&FreeMonoBold24pt7b);
        display.setCursor(settingHourX, settingHourY);

        if (hour < 10)
            display.print("0");

        display.print(hour);

        // Deux-points
        display.setCursor(settingSeparatorX, settingSeparatorY);
        display.print(":");

        // Minutes
        display.setCursor(settingMinuteX, settingMinuteY);

        if (minute < 10)
            display.print("0");

        display.print(minute);

    }
    while (display.nextPage());

    display.hibernate();
}


void screen_show_alarm_setting_page()
{
    display.setRotation(3);
    display.setFullWindow();
    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);

        display.setTextColor(GxEPD_BLACK);
        display.setFont(&FreeMonoBold24pt7b);

        display.setCursor(20, 40);
        display.print("REGLAGE");

        display.setCursor(20, 75);
        display.print("ALARME");
    }
    while (display.nextPage());

    display.hibernate();
}


void screen_update_setting_hour(uint8_t hour)
{
    display.setPartialWindow(
        settingHourX,
        settingHourY - settingHourH,
        settingHourW,
        settingHourH
    );

    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);

        display.setFont(&FreeMonoBold24pt7b);
        display.setTextColor(GxEPD_BLACK);

        display.setCursor(settingHourX, settingHourY);

        if (hour < 10)
            display.print("0");

        display.print(hour);
    }
    while (display.nextPage());

    display.hibernate();
}


void screen_update_setting_minute(uint8_t minute)
{
    display.setPartialWindow(
        settingMinuteX,
        settingMinuteY - settingMinuteH,
        settingMinuteW,
        settingMinuteH
    );

    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);

        display.setFont(&FreeMonoBold24pt7b);
        display.setTextColor(GxEPD_BLACK);

        display.setCursor(settingMinuteX, settingMinuteY);

        if (minute < 10)
            display.print("0");

        display.print(minute);
    }
    while (display.nextPage());

    display.hibernate();
}