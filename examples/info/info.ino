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

    EEPROMr.sectors(4);
    EEPROMr.begin(4096);
    Serial.println();

    Serial.printf("Total number of sectors in flash    : %u\n", EEPROMr.last()+4);
    Serial.printf("Number of sectors in use for EEPROM : %u\n", EEPROMr.sectors());
    Serial.printf("Sectors used for EEPROM are         : ");
    for (uint32_t i = 0; i < EEPROMr.sectors(); i++) {
        if (i>0) Serial.print(", ");
        Serial.print(EEPROMr.base() - i);
    }
    Serial.println();
    Serial.printf("Current sector                      : %u\n", EEPROMr.current());

}

void loop() {}
