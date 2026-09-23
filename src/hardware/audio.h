#ifndef AUDIO_H
#define AUDIO_H

#include <Arduino.h>

#define NUMBER_OF_SOUNDS 5

// Initialise the audio system
void audio_init();

// Start playing a sound
bool audio_play_sound(uint8_t soundNumber);

// Stop audio playback
void audio_stop();

// Update the audio decoder
void audio_update();

// Check if audio is currently playing
bool audio_is_playing();

#endif