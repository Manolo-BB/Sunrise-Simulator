#include "rtc.h"

RTC_DS3231 rtc;

//Hour at first power on
uint8_t rtcHour = 0;
uint8_t rtcMinute = 0;
uint8_t rtcSecond = 0;

static unsigned long lastSecondUpdate = 0;
static unsigned long lastRtcSync = 0;

static void rtc_sync_from_ds3231()
{
    DateTime now = rtc.now();
    rtcHour = now.hour();
    rtcMinute = now.minute();
    rtcSecond = now.second();

    lastSecondUpdate = millis();
    lastRtcSync = millis();

    Serial.print("Synchronisation RTC : ");

    //Add 0 when the hour or minute is less than 10 to allow 09:06
    if (rtcHour < 10)
        Serial.print("0");

    Serial.print(rtcHour);
    Serial.print(":");

    if (rtcMinute < 10)
        Serial.print("0");

    Serial.println(rtcMinute);
}

void rtc_init()
{
    //Begin I2C communication
    Wire.begin(RTC_SDA, RTC_SCL);
    if (!rtc.begin())
    {
        Serial.println("ERREUR : DS3231 non detecte !");
        return;
    }

    Serial.println("DS3231 detecte.");

    //Fake init for power on THIS LINE HAVE TO BE DELET FOR LAST VERSION
    rtc.adjust(DateTime( 2026, 1, 1, rtcHour, rtcMinute, rtcSecond));
    rtc_sync_from_ds3231();
}

//Software clock managment
void rtc_update()
{
    unsigned long currentMillis = millis();

    if (currentMillis - lastSecondUpdate >= 1000)
    {
        lastSecondUpdate += 1000;
        rtcSecond++;
        if (rtcSecond >= 60)
        {
            rtcSecond = 0;
            rtcMinute++;
            if (rtcMinute >= 60)
            {
                rtcMinute = 0;
                rtcHour++;
                if (rtcHour >= 24)
                {
                    rtcHour = 0;
                }
            }
        }
    }

    //real RTC update every RTC_SYNC_INTERVAL seconds
    if (currentMillis - lastRtcSync >= RTC_SYNC_INTERVAL)
    {
        rtc_sync_from_ds3231();
    }
}


void rtc_set_time(uint8_t hour, uint8_t minute)
{
    if (hour > 23)
        hour = 0;

    if (minute > 59)
        minute = 0;

    DateTime now = rtc.now();

    // Update with DS3231
    rtc.adjust( DateTime(now.year(), now.month(), now.day(), hour, minute, 0));

    rtcHour = hour;
    rtcMinute = minute;
    rtcSecond = 0;

    lastSecondUpdate = millis();
    lastRtcSync = millis();

    Serial.print("new hour : ");

    if (rtcHour < 10)
        Serial.print("0");

    Serial.print(rtcHour);
    Serial.print(":");

    if (rtcMinute < 10)
        Serial.print("0");

    Serial.println(rtcMinute);
}


uint8_t rtc_get_hour()
{
    return rtcHour;
}

uint8_t rtc_get_minute()
{
    return rtcMinute;
}