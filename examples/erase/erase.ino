/*
 *  This sketch deletes all contents from the 4 latest sectors
 */

#include <EEPROM_Rotate.h>

EEPROM_Rotate EEPROMr;

void setup() {

    Serial.begin(115200);
    delay(2000);

    Serial.printf("Erasing all sectors\n");
    EEPROMr.size(4);
    EEPROMr.eraseAll();

}

void loop() {}
