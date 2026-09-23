#ifndef SD_CARD_H
#define SD_CARD_H

#include <Arduino.h>

// Initialise the SD card
bool sd_card_init();


// Check if a file exists on the SD card
bool sd_card_file_exists(const char* filename);

#endif