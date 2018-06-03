/*

EEPROM Rotate 0.1.1

EEPROM wrapper for ESP8266

Copyright (C) 2018 by Xose Pérez <xose dot perez at gmail dot com>

The EEPROM_Rotate library is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

The EEPROM_Rotate library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the EEPROM_Rotate library.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef EEPROM_ROTATE_H
#define EEPROM_ROTATE_H

#include <EEPROM.h>
#include <Stream.h>

#ifdef DEBUG_EEPROM_ROTATE_PORT
#define DEBUG_EEPROM_ROTATE(...) DEBUG_EEPROM_ROTATE_PORT.printf( __VA_ARGS__ )
#else
#define DEBUG_EEPROM_ROTATE(...)
#endif

#define EEPROM_ROTATE_CRC_OFFSET        0   // 2 bytes long
#define EEPROM_ROTATE_COUNTER_OFFSET    2   // 1 byte long

class EEPROM_Rotate: public EEPROMClass {

    public:

        EEPROM_Rotate(void): EEPROMClass(last()), _base(last()) {
            _auto();
        };
        EEPROM_Rotate(uint32_t sector): EEPROMClass(sector), _base(sector) {
            _auto();
        };

        bool pool(uint8_t size);
        bool offset(uint16_t offset);
        uint32_t base();
        uint32_t last();
        uint32_t current();
        uint8_t pool();
        uint8_t reserved();
        bool backup(uint32_t target = 0);

        void begin(size_t size);
        bool commit();

        bool erase(uint32_t sector);
        bool eraseAll();
        void dump(Stream & debug, uint32_t sector = 0);

    protected:

        uint32_t _base = 0;
        uint8_t _pool_size = 1;
        uint16_t _offset = 0;
        uint8_t _sector_index = 0;
        uint8_t _sector_value = 0;

        void _auto();
        uint32_t _getSector(uint8_t index);
        uint8_t _getIndex(uint32_t sector);
        uint16_t _calculateCRC();
        bool _checkCRC();

};

#endif // EEPROM_ROTATE_H
