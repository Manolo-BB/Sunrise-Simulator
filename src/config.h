#ifndef CONFIG_H
#define CONFIG_H

/*Basic ESP32 SPI configuration used for the diplay and SD module (see SPI.h configuration)
SCL(SCK)=18
SDA(MOSI)=23
*/

//Pins 2, 5, 12, 15  use for boot strap but works with this application
//Pins 34, 35 are input only

//Display pins
#define DISPLAY_CS          5  
#define DISPLAY_BUSY        22
#define DISPLAY_RES         19
#define DISPLAY_DC          21

//Light pin
#define LIGHT_PIN           17
#define BACKLIGHT_PIN       13

//Button pins
#define SWITCH_LIGHT_PIN    16
#define BUTTON_PLUS_PIN     15
#define BUTTON_MINUS_PIN    32
#define BUTTON_VALIDATE_PIN 4
#define BUTTON_PARAM_PIN    33

//RTC pins
#define RTC_SDA             27  
#define RTC_SCL             14

// Audio amplifier (MAX98357A)
#define AMP_EN              25
#define LRCLK               26
#define BCLK                12  
#define DIN                 34

//SD pins, SCK and MOSI are shared with the display
#define SD_CS               2  
#define SD_MISO             35

#endif