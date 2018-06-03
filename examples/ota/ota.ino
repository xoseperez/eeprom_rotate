/*
 *  This sketch dumps the default EEPROM sector memory
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
        // all but the last one. Calling `backup` without arguments
        // before the upgrade forces the EEPROM_Rotate library to copy
        // the current configuration to the last sector,
        // thus preserving the information.
        EEPROMr.backup();

        Serial.printf("[OTA] Start\n");

    });

    ArduinoOTA.onEnd([]() {
        Serial.printf("[OTA] End\n");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("[OTA] Progress: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("[OTA] Error #%u\n", error);
    });

    ArduinoOTA.begin();

    Serial.printf("[OTA] Ready\n");


}

void setup() {

    Serial.begin(115200);
    delay(2000);
    Serial.println();
    Serial.println();

    // -------------------------------------------------------------------------

    EEPROMr.size(2);
    EEPROMr.begin(256);

    Serial.printf("[EEPROM] Sector pool size : %u\n", EEPROMr.size());
    Serial.printf("[EEPROM] Sectors in use   : ");
    for (uint32_t i = 0; i < EEPROMr.size(); i++) {
        if (i>0) Serial.print(", ");
        Serial.print(EEPROMr.base() - i);
    }
    Serial.println();
    Serial.printf("[EEPROM] Current sector   : %u\n", EEPROMr.current());

    // -------------------------------------------------------------------------

    uint8_t b = EEPROMr.read(10);
    Serial.println();
    Serial.printf("[EEPROM] Current value    : %u\n", b);
    EEPROMr.write(10, b+1);
    EEPROMr.commit();

    // -------------------------------------------------------------------------

    Serial.println();
    wifiSetup();
    otaSetup();
    Serial.println();

}

void loop() {
    ArduinoOTA.handle();
}
