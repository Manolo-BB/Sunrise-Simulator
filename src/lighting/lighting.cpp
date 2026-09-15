#include <Arduino.h>
#include "lighting.h"
#include "config.h"

#define NUM_LEDS    67
#define BRIGHTNESS  30

CRGB leds[NUM_LEDS];

bool lightState = false;


// =========================
// Initialisation
// =========================

void lighting_init()
{
    FastLED.addLeds<WS2812B, LIGHT_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);

    lighting_off();
}


// =========================
// Lumière
// =========================

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


bool lighting_is_on()
{
    return lightState;
}


// =========================
// Couleur
// =========================

void change_color(CRGB color)
{
    fill_solid(leds, NUM_LEDS, color);
    FastLED.show();
}


// =========================
// Luminosité
// =========================

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


int brightness()
{
    return FastLED.getBrightness();
}