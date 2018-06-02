/*
 *  This sketch shows basic info about the configuration
 */

#include <EEPROM_Rotate.h>
#include <spi_flash.h>

extern "C" uint32_t _SPIFFS_start;
extern "C" uint32_t _SPIFFS_end;

EEPROM_Rotate EEPROMr;

// -----------------------------------------------------------------------------

unsigned int sectors(size_t size) {
    return (int) (size + SPI_FLASH_SEC_SIZE - 1) / SPI_FLASH_SEC_SIZE;
}

unsigned long otaSpace() {
    return (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
}

unsigned long eepromSpace() {
    return ESP.getFlashChipSize() - (sectors(ESP.getSketchSize()) + sectors(ESP.getFreeSketchSpace()) + 4) * SPI_FLASH_SEC_SIZE - fsSpace();
}

unsigned long fsSpace() {
    return ((uint32_t)&_SPIFFS_end - (uint32_t)&_SPIFFS_start);
}

void memoryLayoutLine(const char * name, unsigned long bytes, bool update) {
    static unsigned long index = 0;
    if (0 == bytes) return;
    unsigned int _sectors = sectors(bytes);
    Serial.printf("[LAYOUT] %-20s: %8lu bytes / %4d sectors (%4d to %4d)\n", name, bytes, _sectors, index, index + _sectors - 1);
    if (update) index += _sectors;
}

void memoryLayoutLine(const char * name, unsigned long bytes) {
    memoryLayoutLine(name, bytes, true);
}

// -----------------------------------------------------------------------------

void setup() {

    Serial.begin(115200);
    Serial.println();
    Serial.println();
    delay(2000);

    memoryLayoutLine("Flash size (CHIP)", ESP.getFlashChipRealSize(), false);
    memoryLayoutLine("Flash size (SDK)", ESP.getFlashChipSize(), false);
    memoryLayoutLine("Reserved", 1 * SPI_FLASH_SEC_SIZE);
    memoryLayoutLine("Firmware size", ESP.getSketchSize());
    memoryLayoutLine("Max OTA size", otaSpace());
    memoryLayoutLine("SPIFFS size", fsSpace());
    memoryLayoutLine("EEPROM size", eepromSpace());
    memoryLayoutLine("Reserved", 4 * SPI_FLASH_SEC_SIZE);
    Serial.println();

    // Not needed if we have already reserved several sectors in the memory layout
    // EEPROMr.pool(2);

    // Load data
    EEPROMr.begin(4096);

    Serial.printf("[EEPROM] Sector pool size : %u\n", EEPROMr.pool());
    Serial.printf("[EEPROM] Sectors in use   : ");
    for (uint32_t i = 0; i < EEPROMr.pool(); i++) {
        if (i>0) Serial.print(", ");
        Serial.print(EEPROMr.base() - i);
    }
    Serial.println();
    Serial.printf("[EEPROM] Current sector   : %u\n", EEPROMr.current());

}

void loop() {}
