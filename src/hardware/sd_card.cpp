#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

#include "sd_card.h"
#include "../config.h"

// Initialise the SD card
bool sd_card_init()
{
    // Configure the shared SPI bus
    SPI.begin(18, SD_MISO, 23, SD_CS);

    // Initialise the SD card
    if (!SD.begin(SD_CS, SPI))
    {
        return false;
    }

    return true;
}


// Check if a file exists on the SD card
bool sd_card_file_exists(const char* filename)
{
    return SD.exists(filename);
}