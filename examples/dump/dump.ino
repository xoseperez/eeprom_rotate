/*
 *  This sketch dumps the default EEPROM sector memory
 */

#include <EEPROM_Rotate.h>

EEPROM_Rotate EEPROMr;

void setup() {

    // DEBUG -------------------------------------------------------------------

    Serial.begin(115200);
    delay(2000);
    Serial.println();
    Serial.println();

    // EEPROM Initialization ---------------------------------------------------

    EEPROMr.begin(4096);

    // Example -----------------------------------------------------------------

    Serial.println();
    Serial.printf("[EEPROM] Dumping data for sector #%u\n", EEPROMr.current());
    EEPROMr.dump(Serial);

}

void loop() {}
