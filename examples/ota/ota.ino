/*
 *  This sketch shows how to backup EEPROM data before an OTA upgrade
 */

#include <EEPROM_Rotate.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include "credentials.h"

EEPROM_Rotate EEPROMr;

void wifiSetup() {

    // Set WIFI module to STA mode
    WiFi.mode(WIFI_STA);

    // Connect
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Wait
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    // Connected!
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

}

void otaSetup() {

    ArduinoOTA.onStart([]() {

        // If we are not specifically reserving the sectors we are using as
        // EEPROM in the memory layout then any OTA upgrade will overwrite
        // all but the last one.
        // Calling rotate(false) disables rotation so all writes will be done
        // to the last sector. It also sets the dirty flag to true so the next commit()
        // will actually persist current configuration to that last sector.
        // Calling rotate(false) will also prevent any other EEPROM write
        // to overwrite the OTA image.
        // In case the OTA process fails, reenable rotation.
        // See onError callback below.
        EEPROMr.rotate(false);
        EEPROMr.commit();

        Serial.printf("[OTA] Start\n");

    });

    ArduinoOTA.onEnd([]() {
        Serial.printf("[OTA] End\n");
        // Here the board will reboot
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("[OTA] Progress: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("[OTA] Error #%u\n", error);

        // There's been an error, reenable rotation
        EEPROMr.rotate(true);

    });

    ArduinoOTA.begin();

    Serial.printf("[OTA] Ready\n");

}

void setup() {

    // DEBUG -------------------------------------------------------------------

    Serial.begin(115200);
    delay(2000);
    Serial.println();
    Serial.println();

    // EEPROM Initialization ---------------------------------------------------

    EEPROMr.size(4);
    EEPROMr.begin(256);

    Serial.printf("[EEPROM] Sector pool size : %u\n", EEPROMr.size());
    Serial.printf("[EEPROM] Sectors in use   : ");
    for (uint32_t i = 0; i < EEPROMr.size(); i++) {
        if (i>0) Serial.print(", ");
        Serial.print(EEPROMr.base() - i);
    }
    Serial.println();
    Serial.printf("[EEPROM] Current sector   : %u\n", EEPROMr.current());

    uint8_t b = EEPROMr.read(10);
    Serial.println();
    Serial.printf("[EEPROM] Current value    : %u\n", b);
    EEPROMr.write(10, b+1);
    EEPROMr.commit();

    // WiFi & OTA Initialization -----------------------------------------------

    Serial.println();
    wifiSetup();
    otaSetup();
    Serial.println();

}

void loop() {
    ArduinoOTA.handle();
}
