
//Global libraries
#include <Arduino.h>
#include "config.h"
#include "lighting.h"

#define NUM_LEDS    67 //Number of Leds on the strip
#define BRIGHTNESS  30 //Initialisation brightness at first power on

CRGB leds[NUM_LEDS];
bool lightState = false;

//Initialisation with FastLED lib
void lighting_init()
{
    FastLED.addLeds<WS2812B, LIGHT_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    pinMode(BACKLIGHT_PIN, OUTPUT);
    digitalWrite(BACKLIGHT_PIN, LOW);
    lighting_off();
}

void lighting_on(CRGB color)
{
    fill_solid(leds, NUM_LEDS, color);
    FastLED.show();

    lightState = true;
}

void lighting_off()
{
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();

    lightState = false;
}

void change_color(CRGB color)
{
    fill_solid(leds, NUM_LEDS, color);
    FastLED.show();
}

bool lighting_is_on()
{
    return lightState;
}

void increase_brightness()
{
    int currentBrightness = FastLED.getBrightness();

    currentBrightness += 5;

    if (currentBrightness > 255)
    {
        currentBrightness = 255;
    }

    FastLED.setBrightness(currentBrightness);
    FastLED.show();
}

void decrease_brightness()
{
    int currentBrightness = FastLED.getBrightness();

    currentBrightness -= 5;

    if (currentBrightness < 5)
    {
        currentBrightness = 5;
    }

    FastLED.setBrightness(currentBrightness);
    FastLED.show();
}

//Return the actual brightness
int brightness()
{
    return FastLED.getBrightness();
}

//For sunrise simulation
void set_brightness(uint8_t value)
{
    FastLED.setBrightness(value);
}

