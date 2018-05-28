/*
 *  This sketch shows basic info about the configuration
 */

#include <EEPROM_Rotate.h>

EEPROM_Rotate EEPROMr;

void setup() {

    Serial.begin(115200);
    Serial.println();
    Serial.println();
    delay(2000);

    EEPROMr.pool(4);
    EEPROMr.begin(4096);
    Serial.println();

    Serial.printf("Total sectors    : %u\n", EEPROMr.last()+4);
    Serial.printf("Sector pool size : %u\n", EEPROMr.pool());
    Serial.printf("Sectors in use   : ");
    for (uint32_t i = 0; i < EEPROMr.pool(); i++) {
        if (i>0) Serial.print(", ");
        Serial.print(EEPROMr.base() - i);
    }
    Serial.println();
    Serial.printf("Current sector   : %u\n", EEPROMr.current());

}

void loop() {}
