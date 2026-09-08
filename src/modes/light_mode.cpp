#include <Arduino.h>

#include "light_mode.h"
#include "../lighting/lighting.h"
#include "../hardware/button.h"

#define VALIDATION_TIME 1000
#define NUMBER_OF_COLORS 7

CRGB colors[NUMBER_OF_COLORS] =
{
    CRGB::Red,
    CRGB::Green,
    CRGB::Blue,
    CRGB::Orange,
    CRGB::Purple,
    CRGB::Cyan,
    CRGB::White  
};

const char* colorNames[NUMBER_OF_COLORS] =
{
    "Red",
    "Green",
    "Blue",
    "Orange",
    "Purple",
    "Cyan",
    "White"    
};

bool settingMode = false;

// false = color choice
// true  = brightness adjustment
bool brightnessMode = false;

int selectedColor = 0;

unsigned long validatePressStart = 0;
bool validateButtonWasPressed = false;


void light_mode_init()
{
    Serial.println("Mode lumiere initialise");
}

// Update the LED's colour and brightness
void light_mode_update()
{
    buttons_update();

    //Switch light
    if (button_just_pressed(SWITCH_LIGHT))
    {
        if (lighting_is_on())
        {
            lighting_off();

            //Reset the setting mode and validation state when the light is turned off
            settingMode = false;
            validateButtonWasPressed = false;

            Serial.println("Lumiere OFF");
        }
        else
        {
            lighting_on(colors[selectedColor]);

            Serial.println("Lumiere ON");
        }
        return;
    }

    // Setting mode
    if (lighting_is_on())
    {
        //First press on a button, enter setting mode
        if (!settingMode)
        {
            if (button_just_pressed(BUTTON_PLUS))
            {
                settingMode = true;
                brightnessMode = false;

                selectedColor++;

                if (selectedColor >= NUMBER_OF_COLORS)
                {
                    selectedColor = 0;
                }

                change_color(colors[selectedColor]);

                Serial.print("Choix couleur : ");
                Serial.println(colorNames[selectedColor]);
            }

            else if (button_just_pressed(BUTTON_MINUS))
            {
                settingMode = true;
                brightnessMode = false;

                selectedColor--;

                if (selectedColor < 0)
                {
                    selectedColor = NUMBER_OF_COLORS - 1;
                }

                change_color(colors[selectedColor]);

                Serial.print("Choix couleur : ");
                Serial.println(colorNames[selectedColor]);
            }
        }
        // Other presses, change color or brightness
        else if (!brightnessMode)
        {
            if (button_just_pressed(BUTTON_PLUS))
            {
                selectedColor++;

                if (selectedColor >= NUMBER_OF_COLORS)
                {
                    selectedColor = 0;
                }

                change_color(colors[selectedColor]);

                Serial.print("Couleur : ");
                Serial.println(colorNames[selectedColor]);
            }

            else if (button_just_pressed(BUTTON_MINUS))
            {
                selectedColor--;

                if (selectedColor < 0)
                {
                    selectedColor = NUMBER_OF_COLORS - 1;
                }

                change_color(colors[selectedColor]);

                Serial.print("Couleur : ");
                Serial.println(colorNames[selectedColor]);
            }
        }

        else
        {
            if (button_just_pressed(BUTTON_PLUS))
            {
                increase_brightness();

                Serial.print("Luminosite : ");
                Serial.println(brightness());
            }

            else if (button_just_pressed(BUTTON_MINUS))
            {
                decrease_brightness();

                Serial.print("Luminosite : ");
                Serial.println(brightness());
            }
        }
    }

    // Validation of the brightness setting
    if (settingMode)
    {
        bool validatePressed = button_pressed(BUTTON_VALIDATE);


        // Start pressing the validation button
        if (validatePressed && !validateButtonWasPressed)
        {
            validatePressStart = millis();

            validateButtonWasPressed = true;

            Serial.println("Validation en cours...");
        }


        // End of the press
        else if (!validatePressed && validateButtonWasPressed)
        {
            unsigned long pressDuration = millis() - validatePressStart;

            validateButtonWasPressed = false;

            if (pressDuration >= VALIDATION_TIME)
            {
                if (!brightnessMode)
                {
                    // Color validated, enter brightness adjustment mode
                    brightnessMode = true;

                    Serial.print("Color validated : ");
                    Serial.println(colorNames[selectedColor]);

                    Serial.println("Brightness adjustment mode");
                }
                else
                {
                    // Brightness validated
                    settingMode = false;

                    Serial.println("SETTINGS COMPLETE !");
                }
            }
            else
            {
                Serial.println("Appui trop court");
            }
        }
    }
}