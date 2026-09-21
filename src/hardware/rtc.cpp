#include "rtc.h"

RTC_DS3231 rtc;

//Hour at first power on
uint8_t currentHour = 0;
uint8_t currentMinute = 0;
uint8_t currentSecond = 0;

static unsigned long lastSecondUpdate = 0;
static unsigned long lastRtcSync = 0;

static void rtc_sync_from_ds3231()
{
    DateTime now = rtc.now();
    currentHour = now.hour();
    currentMinute = now.minute();
    currentSecond = now.second();

    lastSecondUpdate = millis();
    lastRtcSync = millis();

    Serial.print("Synchronisation RTC : ");

    //Add 0 when the hour or minute is less than 10 to allow 09:06
    if (currentHour < 10)
        Serial.print("0");

    Serial.print(currentHour);
    Serial.print(":");

    if (currentMinute < 10)
        Serial.print("0");

    Serial.println(currentMinute);
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

    //Fake init for power on
    rtc.adjust(DateTime( 2026, 1, 1, currentHour, currentMinute, currentSecond));
    rtc_sync_from_ds3231();
}

//Software clock managment
void rtc_update()
{
    unsigned long currentMillis = millis();

    if (currentMillis - lastSecondUpdate >= 1000)
    {
        lastSecondUpdate += 1000;
        currentSecond++;
        if (currentSecond >= 60)
        {
            currentSecond = 0;
            currentMinute++;
            if (currentMinute >= 60)
            {
                currentMinute = 0;
                currentHour++;
                if (currentHour >= 24)
                {
                    currentHour = 0;
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

    currentHour = hour;
    currentMinute = minute;
    currentSecond = 0;

    lastSecondUpdate = millis();
    lastRtcSync = millis();

    Serial.print("new hour : ");

    if (currentHour < 10)
        Serial.print("0");

    Serial.print(currentHour);
    Serial.print(":");

    if (currentMinute < 10)
        Serial.print("0");

    Serial.println(currentMinute);
}


uint8_t rtc_get_hour()
{
    return currentHour;
}

uint8_t rtc_get_minute()
{
    return currentMinute;
}