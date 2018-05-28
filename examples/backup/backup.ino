/*
 *  This sketch shows sector hoping acros reboots
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

    Serial.printf("Sector pool size : %u\n", EEPROMr.pool());
    Serial.printf("Sectors in use   : ");
    for (uint32_t i = 0; i < EEPROMr.pool(); i++) {
        if (i>0) Serial.print(", ");
        Serial.print(EEPROMr.base() - i);
    }
    Serial.println();
    Serial.printf("Current sector   : %u\n", EEPROMr.current());

    Serial.println();
    Serial.println("Backup to last sector");
    EEPROMr.backup();
    Serial.println();

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
