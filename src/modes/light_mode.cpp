#include <Arduino.h>
#include "config.h"
#include "light_mode.h"

#include "../hardware/lighting.h"
#include "../hardware/button.h"
#include "modes/menu_mode.h"

//Configuration
#define VALIDATION_TIME 1000
#define NUMBER_OF_COLORS 7

//Available colors
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

// false = normal mode, true = color/brightness setting mode
static bool settingMode = false; 


// false = color selection, true = brightness adjustment
static bool brightnessMode = false;

static int selectedColor = 0;

//Validation button state
static unsigned long validatePressStart = 0;
static bool validateButtonWasPressed = false;

//=============================================
// Static functions
//=============================================

// Synchronize the light output with the physical light switch state
static void switch_management()
{
    if (switch_light_state())
    {
        if (!lighting_is_on())
        {
            lighting_on(colors[selectedColor]);
            Serial.println("Lumiere ON");
        }
    }
    else
    {
        if (lighting_is_on())
        {
            lighting_off();

            // Reset the setting mode when the light is switched off
            settingMode = false;
            brightnessMode = false;
            validateButtonWasPressed = false;

            Serial.println("Lumiere OFF");
        }
    }
}

static void change_color_selection(){
    if (button_just_pressed(BUTTON_PLUS))
        {
            selectedColor++;

            if (selectedColor >= NUMBER_OF_COLORS)
                selectedColor = 0;

            change_color(colors[selectedColor]);
        }
        else if (button_just_pressed(BUTTON_MINUS))
        {
            if (selectedColor == 0)
                selectedColor = NUMBER_OF_COLORS - 1;
            else
                selectedColor--;

            change_color(colors[selectedColor]);
        }
}

//Handle color selection and brightness adjustement
static void setting_mode_update()
{
    if (!lighting_is_on())
        return;

    // Enter setting mode
    if (!settingMode)
    {
        settingMode = true; 
        change_color_selection();
    }

    // Change color
    else if (!brightnessMode)
    {
        change_color_selection();
    }

    // Change brightness
    else
    {
        if (button_just_pressed(BUTTON_PLUS))
        {
            increase_brightness();

            Serial.print("Brightness: ");
            Serial.println(brightness());
        }
        else if (button_just_pressed(BUTTON_MINUS))
        {
            decrease_brightness();

            Serial.print("Brightness: ");
            Serial.println(brightness());
        }
    }
  
    // Long press on validation button
    if (settingMode)
    {
        bool validatePressed = button_pressed(BUTTON_VALIDATE);

        // Start measuring the validation button press
        if (validatePressed && !validateButtonWasPressed)
        {
            validatePressStart = millis();
            validateButtonWasPressed = true;

            Serial.println("Validation in progress...");
        }

        // The validation button has been released
        else if (!validatePressed && validateButtonWasPressed)
        {
            unsigned long pressDuration = millis() - validatePressStart;
            validateButtonWasPressed = false;

            if (pressDuration >= VALIDATION_TIME)
            {
                if (!brightnessMode)
                {
                    // The color is validated and brightness adjustment starts
                    brightnessMode = true;

                    Serial.print("Color validated: ");
                    Serial.println(colorNames[selectedColor]);
                    Serial.println("Brightness adjustment mode");
                }
                else
                {
                    // The brightness is validated and settings are complete
                    settingMode = false;

                    Serial.println("Settings complete");
                }
            }
            else
            {
                Serial.println("Press too short");
            }
        }
    }
}

//=============================================
// Global functions
//=============================================

//Initialisation
void light_mode_init()
{
    lighting_init();
    Serial.println("light mode initialised");
}

// Update the LED's colour and brightness
void light_mode_update()
{
    switch_management();
    setting_mode_update();
}

// Turn on the backlight when VALIDATE_BUTTON is pressed and light is off
void backlight_update()
{
    static unsigned long backlightStartTime = 0;
    static bool backlightActive = false;

    if (switch_light_state() || menu_is_active())
    {
        digitalWrite(BACKLIGHT_PIN, LOW);
        backlightActive = false;
        return;
    }
    if (button_just_pressed(BUTTON_VALIDATE) && !switch_light_state())
    {
        digitalWrite(BACKLIGHT_PIN, HIGH);

        backlightStartTime = millis();
        backlightActive = true;
    }
    if (backlightActive && millis() - backlightStartTime >= 3000)
    {
        digitalWrite(BACKLIGHT_PIN, LOW);
        backlightActive = false;
    }
}