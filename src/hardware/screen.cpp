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

static int fakeHour = 7;
static int fakeMinute = 30;
static unsigned long fakeLastUpdate = 0;

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
}

//Hour and minute display
void screen_show_time(int hour, int minute)
{
    char timeText[6];

    snprintf(timeText, sizeof(timeText),"%02d:%02d", hour, minute);

    display.setRotation(3);
    display.setFullWindow();
    display.setFont(&FreeMonoBold24pt7b);
    display.setTextColor(GxEPD_BLACK);

    int16_t tbx;
    int16_t tby;
    uint16_t tbw;
    uint16_t tbh;

    display.getTextBounds( timeText, 0, 0, &tbx, &tby,&tbw, &tbh);

    // Center horizontally and vertically
    int16_t x =
        (display.width() - tbw) / 2 - tbx;

    int16_t y =
        (display.height() - tbh) / 2 - tby;

    display.firstPage();

    do
    {
        display.fillScreen(GxEPD_WHITE);

        display.setTextColor(GxEPD_BLACK);
        display.setFont(&FreeMonoBold24pt7b);

        display.setCursor(x, y);

        display.print(timeText);
    }
    while (display.nextPage());
}


//Add a function to draw an alarm icon with a number in the center
//This function was maid by an LLM
static void draw_alarm_icon(
    int16_t x,
    int16_t y,
    uint8_t alarmNumber
)
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
        x + 4, y + 7,
        x + 0, y + 2,
        x + 7, y + 4,
        GxEPD_BLACK
    );

    // Right bell
    display.fillTriangle(
        x + 24, y + 7,
        x + 28, y + 2,
        x + 21, y + 4,
        GxEPD_BLACK
    );

    // Draw the alarm number in the center of the icon
    char numberText[3];

    snprintf( numberText, sizeof(numberText), "%d", alarmNumber);

    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(GxEPD_WHITE);

    int16_t tbx;
    int16_t tby;
    uint16_t tbw;
    uint16_t tbh;

    display.getTextBounds( numberText, 0, 0, &tbx, &tby,&tbw, &tbh);

    int16_t textX =
        x + 14 - tbw / 2 - tbx;

    int16_t textY =
        y + 14 - tbh / 2 - tby;

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
        draw_alarm_icon( 5, 5, alarmNumber);
    }
    while (display.nextPage());
}

// Display active alarms
void screen_show_active_alarms( bool alarm1, bool alarm2, bool alarm3, bool alarm4, bool alarm5 ){
    
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
}

void screen_show_home(
    int hour,
    int minute,
    bool alarm1,
    bool alarm2,
    bool alarm3,
    bool alarm4,
    bool alarm5
)
{
    char timeText[6];

    snprintf(
        timeText,
        sizeof(timeText),
        "%02d:%02d",
        hour,
        minute
    );

    display.setRotation(3);
    display.setFont(&FreeMonoBold24pt7b);
    display.setTextColor(GxEPD_BLACK);

    // Calcul de la position de l'heure
    int16_t tbx, tby;
    uint16_t tbw, tbh;

    
    display.getTextBounds( timeText, 0, 0, &tbx, &tby, &tbw, &tbh);

    int16_t timeX =
        (display.width() - tbw) / 2 - tbx;

    int16_t timeY =
        (display.height() - tbh) / 2 - tby;

    static bool firstDisplay = true;

    if (firstDisplay)
    {
        firstDisplay = false;
        display.setFullWindow();
        display.firstPage();

        do
        {
            //HOUR
            display.fillScreen(GxEPD_WHITE);
            display.setFont(&FreeMonoBold24pt7b);
            display.setTextColor(GxEPD_BLACK);

            display.setCursor(timeX, timeY);
            display.print(timeText);


            // Active alarms
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

        return;
    }

    const int16_t timeWindowX = 55;
    const int16_t timeWindowY = 40;
    const uint16_t timeWindowW = 266;
    const uint16_t timeWindowH = 80;

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

        display.setFont(&FreeMonoBold24pt7b);
        display.setTextColor(GxEPD_BLACK);

        display.setCursor(timeX, timeY);
        display.print(timeText);

    }
    while (display.nextPage());
}

// fake screen update for testing purposes
void fake_screen()
{
    if (millis() - fakeLastUpdate >= 10000 || fakeLastUpdate == 0)
    {
        fakeLastUpdate = millis();

        screen_show_home(
            fakeHour,
            fakeMinute,
            true,   // réveil 1
            false,  // réveil 2
            true,  // réveil 3
            true,  // réveil 4
            false    // réveil 5
        );

        fakeMinute++;

        if (fakeMinute >= 60)
        {
            fakeMinute = 0;
            fakeHour++;

            if (fakeHour >= 24)
                fakeHour = 0;
        }
    }
}