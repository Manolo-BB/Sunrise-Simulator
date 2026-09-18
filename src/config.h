#ifndef CONFIG_H
#define CONFIG_H

/*Basic ESP32 SPI configuration
SCL(SCK)=18
SDA(MOSI)=23
*/
//Display pins
#define DISPLAY_CS 5
#define DISPLAY_BUSY 22
#define DISPLAY_RES 19
#define DISPLAY_DC 21

//Light pin
#define LIGHT_PIN   17
#define BACKLIGHT_PIN  13

//Button pins
#define SWITCH_LIGHT_PIN    16
#define BUTTON_PLUS_PIN     15
#define BUTTON_MINUS_PIN    32
#define BUTTON_VALIDATE_PIN 4
#define BUTTON_PARAM_PIN    33

//RTC pins
#define RTC_SDA 27  
#define RTC_SCL 14

#endif