/*
 *  This sketch dumps the default EEPROM sector memory
 */

#include <EEPROM_Rotate.h>

EEPROM_Rotate EEPROMr;

void setup() {

    Serial.begin(115200);
    delay(2000);
    Serial.println();
    Serial.println();

    EEPROMr.begin(4096);
    Serial.printf("[EEPROM] Dumping data for sector #%u\n", EEPROMr.current());
    EEPROMr.dump(Serial);

}

void loop() {}
