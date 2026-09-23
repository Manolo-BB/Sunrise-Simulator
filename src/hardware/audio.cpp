#include <Arduino.h>

#include "audio.h"
#include "sd_card.h"
#include "config.h"

#include "AudioFileSourceSD.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"


// Audio objects
static AudioGeneratorMP3* mp3 = nullptr;
static AudioFileSourceSD* file = nullptr;
static AudioOutputI2S* output = nullptr;

// Currently selected sound
static uint8_t currentSound = 0;

// Initialise the audio system
void audio_init()
{
    pinMode(AMP_EN, OUTPUT);
    digitalWrite(AMP_EN, HIGH);
    
    // Create the I2S output
    output = new AudioOutputI2S();

    // Configure I2S pins:
    // BCLK  = GPIO12
    // LRCLK = GPIO26
    // DIN   = GPIO1
    output->SetPinout(BCLK, LRCLK, DIN);

    // Set initial audio gain
    output->SetGain(0.5);
}

// Start playing a sound
bool audio_play_sound(uint8_t soundNumber)
{
    if (soundNumber < 1 || soundNumber > NUMBER_OF_SOUNDS)
    {
        return false;
    }

    // Stop the current sound if necessary
    audio_stop();

    // Build the filename
    char filename[20];
    snprintf(filename, sizeof(filename), "/alarm%u.mp3", soundNumber);

    // Check that the file exists
    if (!sd_card_file_exists(filename))
    {
        return false;
    }

    // Store the selected sound
    currentSound = soundNumber;

    // Open the MP3 file
    file = new AudioFileSourceSD(filename);

    // Create the MP3 decoder
    mp3 = new AudioGeneratorMP3();

    // Start decoding
    if (!mp3->begin(file, output))
    {
        delete mp3;
        delete file;

        mp3 = nullptr;
        file = nullptr;

        return false;
    }

    return true;
}

// Stop audio playback
void audio_stop()
{
    if (mp3 != nullptr)
    {
        if (mp3->isRunning())
        {
            mp3->stop();
        }

        delete mp3;
        mp3 = nullptr;
    }

    if (file != nullptr)
    {
        delete file;
        file = nullptr;
    }

    currentSound = 0;
}

// Update the audio decoder
void audio_update()
{
    if (mp3 == nullptr)
    {
        return;
    }

    if (mp3->isRunning())
    {
        // Continue decoding the MP3
        if (!mp3->loop())
        {
            mp3->stop();
        }

        return;
    }

    // Restart the selected sound when it reaches the end
    if (currentSound != 0)
    {
        uint8_t soundToRestart = currentSound;

        audio_stop();
        audio_play_sound(soundToRestart);
    }
}

// Check if audio is currently playing
bool audio_is_playing()
{
    if (mp3 == nullptr)
    {
        return false;
    }

    return mp3->isRunning();
}