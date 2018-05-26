/*
 *  This sketch dumps the default EEPROM sector memory
 */

#include <EEPROM_Rotate.h>

EEPROM_Rotate EEPROMr;

void setup() {

    Serial.begin(115200);
    delay(2000);

    EEPROMr.begin(4096);
    EEPROMr.dump(Serial);

}

void loop() {}
