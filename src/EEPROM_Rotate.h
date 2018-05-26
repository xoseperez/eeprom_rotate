/*

EEPROM Rotate 0.1.0

EEPRIM wrapper for ESP8266

Copyright (C) 2018 by Xose Pérez <xose dot perez at gmail dot com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef EEPROM_ROTATE_H
#define EEPROM_ROTATE_H

#include <EEPROM.h>
#include <Stream.h>
#include <spi_flash.h>

#ifdef DEBUG_EEPROM_ROTATE_PORT
#define DEBUG_EEPROM_ROTATE(...) DEBUG_EEPROM_ROTATE_PORT.printf( __VA_ARGS__ )
#else
#define DEBUG_EEPROM_ROTATE(...)
#endif

#define EEPROM_ROTATE_CRC_OFFSET        0   // 2 bytes long
#define EEPROM_ROTATE_COUNTER_OFFSET    2   // 1 byte long

class EEPROM_Rotate: public EEPROMClass {

    public:

        EEPROM_Rotate(uint32_t sector = 0);

        bool rotate(uint8_t sectors);
        bool offset(uint8_t offset);
        uint8_t last();

        void begin(size_t size);
        bool commit();

        bool erase(uint32_t sector);
        bool eraseAll();
        void dump(Stream & debug);

    private:

        uint8_t _sectors = 1;
        uint16_t _offset = 0;
        uint8_t _sector_index = 0;
        uint8_t _sector_value = 0;

        uint32_t _getLastSector();
        uint32_t _getSector(uint8_t index);
        uint16_t _calculateCRC();
        bool _checkCRC();

};

#endif // EEPROM_ROTATE_H
