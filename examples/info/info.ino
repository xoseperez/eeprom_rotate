/*
 *  This sketch shows basic info about the configuration
 */

#include <EEPROM_Rotate.h>
#include <spi_flash.h>

EEPROM_Rotate EEPROMr;

// -----------------------------------------------------------------------------
// INFO
// -----------------------------------------------------------------------------

extern "C" uint32_t _SPIFFS_start;
extern "C" uint32_t _SPIFFS_end;

unsigned int info_bytes2sectors(size_t size) {
    return (int) (size + SPI_FLASH_SEC_SIZE - 1) / SPI_FLASH_SEC_SIZE;
}

unsigned long info_ota_space() {
    return (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
}

unsigned long info_filesystem_space() {
    return ((uint32_t)&_SPIFFS_end - (uint32_t)&_SPIFFS_start);
}

unsigned long info_eeprom_space() {
    return EEPROMr.reserved() * SPI_FLASH_SEC_SIZE;
}

void _info_print_memory_layout_line(const char * name, unsigned long bytes, bool reset) {
    static unsigned long index = 0;
    if (reset) index = 0;
    if (0 == bytes) return;
    unsigned int _sectors = info_bytes2sectors(bytes);
    Serial.printf("[INIT] %-20s: %8lu bytes / %4d sectors (%4d to %4d)\n", name, bytes, _sectors, index, index + _sectors - 1);
    index += _sectors;
}

void _info_print_memory_layout_line(const char * name, unsigned long bytes) {
    _info_print_memory_layout_line(name, bytes, false);
}

// -----------------------------------------------------------------------------

void setup() {

    // DEBUG -------------------------------------------------------------------

    Serial.begin(115200);
    delay(2000);
    Serial.println();
    Serial.println();

    // Memory layout info ------------------------------------------------------

    _info_print_memory_layout_line("Flash size (CHIP)", ESP.getFlashChipRealSize(), true);
    _info_print_memory_layout_line("Flash size (SDK)", ESP.getFlashChipSize(), true);
    _info_print_memory_layout_line("Reserved", 1 * SPI_FLASH_SEC_SIZE, true);
    _info_print_memory_layout_line("Firmware size", ESP.getSketchSize());
    _info_print_memory_layout_line("Max OTA size", info_ota_space());
    _info_print_memory_layout_line("SPIFFS size", info_filesystem_space());
    _info_print_memory_layout_line("EEPROM size", info_eeprom_space());
    _info_print_memory_layout_line("Reserved", 4 * SPI_FLASH_SEC_SIZE);

    Serial.println();

    // EEPROM Initialization ---------------------------------------------------

    // Not needed if we have already reserved several sectors in the memory layout
    EEPROMr.size(4);

    // Load data
    EEPROMr.begin(4096);

    // EEPROM Info -------------------------------------------------------------

    Serial.println();
    Serial.printf("[EEPROM] Reserved sectors : %u\n", EEPROMr.reserved());
    Serial.printf("[EEPROM] Sector pool size : %u\n", EEPROMr.size());
    Serial.printf("[EEPROM] Sectors in use   : ");
    for (uint32_t i = 0; i < EEPROMr.size(); i++) {
        if (i>0) Serial.print(", ");
        Serial.print(EEPROMr.base() - i);
    }
    Serial.println();
    Serial.printf("[EEPROM] Current sector   : %u\n", EEPROMr.current());

}

void loop() {}
