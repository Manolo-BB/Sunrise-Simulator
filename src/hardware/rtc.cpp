//Global libraries
#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include "config.h"
#include "rtc.h"

// The RTC update takes place every 10 hours
#define RTC_SYNC_INTERVAL 36000000UL

//Rename the RTC module as rtc
RTC_DS3231 rtc;

//Hour at first power on after that, RTC manage it
uint8_t currentHour = 0;
uint8_t currentMinute = 0;
uint8_t currentSecond = 0;

//Variables used to count update intervals
static unsigned long lastSecondUpdate = 0;
static unsigned long lastRtcSync = 0;

//Every RTC_SYNC_INTERVAL we update the software hour with the real RTC
static void rtc_sync_from_ds3231()
{
    DateTime now = rtc.now();
    currentHour = now.hour();
    currentMinute = now.minute();
    currentSecond = now.second();

    lastSecondUpdate = millis();
    lastRtcSync = millis();

    //Debugging print
    Serial.print("Synchronisation RTC : ");

    //Add 0 when the hour or minute is less than 10 to allow 09:06
    if (currentHour < 10)
    {
        Serial.print("0");  
    }
        
    Serial.print(currentHour);
    Serial.print(":");

    if (currentMinute < 10)
    {
        Serial.print("0");  
    }

    Serial.println(currentMinute);
}

//Initialisation of the RTC module
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
    
    //Carry out an synchronisation if the circuit has been disconnected
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
    DateTime now = rtc.now();

    // Update only hour and minutes
    rtc.adjust( DateTime(now.year(), now.month(), now.day(), hour, minute, 0));

    // Synch variables
    currentHour = hour;
    currentMinute = minute;
    currentSecond = 0;

    lastSecondUpdate = millis();
    lastRtcSync = millis();

    //Debugging print
    Serial.print("new hour : ");

    if (currentHour < 10)
        Serial.print("0");

    Serial.print(currentHour);
    Serial.print(":");

    if (currentMinute < 10)
        Serial.print("0");

    Serial.println(currentMinute);
}

void rtc_set_date(uint8_t day, uint8_t month, uint16_t year)
{
    //We retrieve the variables
    DateTime now = rtc.now();

    //We only update the date variables, the others remain unchanged.
    rtc.adjust(DateTime(year, month, day, rtc_get_hour(), rtc_get_minute(), currentSecond));

    // Debugging print
    Serial.print("Nouvelle date : ");
    Serial.print(day);
    Serial.print("/");
    Serial.print(month);
    Serial.print("/");
    Serial.println(year);
}

//Getters to get variables
uint8_t rtc_get_hour()
{
    return currentHour;
}

uint8_t rtc_get_minute()
{
    return currentMinute;
}

uint8_t rtc_get_second()
{
    return currentSecond;
}

uint8_t rtc_get_day()
{
    DateTime now = rtc.now();
    return now.day();
}

uint8_t rtc_get_month()
{
    DateTime now = rtc.now();
    return now.month();
}

uint16_t rtc_get_year()
{
    DateTime now = rtc.now();
    return now.year();
}

uint8_t rtc_get_weekday()
{
    DateTime now = rtc.now();

    return now.dayOfTheWeek();
}