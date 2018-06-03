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

#include "Arduino.h"
#include "EEPROM_Rotate.h"

extern "C" {
    #include "spi_flash.h"
}

extern "C" uint32_t _SPIFFS_end;

// -----------------------------------------------------------------------------
// PUBLIC *NEW* METHODS
// -----------------------------------------------------------------------------

/**
 * @brief Defines the sector pool size that will be used for EEPROM rotation.
 * Must be called before the begin method.
 * @param {uint8_t} sectors     Number of sectors (from 1 to 10)
 * @returns {bool}              True if seccessfully set
 */
bool EEPROM_Rotate::size(uint8_t size) {
    if (size < 1 || 10 < size) return false;
    _pool_size = size;
    return true;
}

/**
 * @brief Defines the offset inside the sector memory where the magic bytes will be stored.
 * The library uses 3 bytes to track last valid sector, so there must be at least 3
 * bytes available in the memory buffer from the offset onwards.
 * Must be called before the begin method.
 * @param {uint16_t} offset     Offset
 * @returns {bool}              True if seccessfully set
 */
bool EEPROM_Rotate::offset(uint16_t offset) {
    if (offset + 3 > SPI_FLASH_SEC_SIZE) return false;
    _offset = offset;
    return true;
}

/**
 * @brief Returns the number of the last available sector for EEPROM.
 * This is also the sector that the default EEPROM library uses.
 * @returns {uint32_t}          Last available sector for EEPROM storing
 */
uint32_t EEPROM_Rotate::last() {
    return ESP.getFlashChipSize() / SPI_FLASH_SEC_SIZE - 5;
}

/**
 * @brief Returns the base sector for current rotating configuration.
 * @returns {uint32_t}          Base sector
 */
uint32_t EEPROM_Rotate::base() {
    return _base;
}

/**
 * @brief Returns the sector index whose contents match those of the EEPROM data buffer.
 * @returns {uint32_t}          Current sector
 */
uint32_t EEPROM_Rotate::current() {
    return _sector;
}

/**
 * @brief Returns the number of sectors used for rotating EEPROM.
 * @returns {uint8_t}           Sector pool size
 */
uint8_t EEPROM_Rotate::size() {
    return _pool_size;
}

/**
 * @brief Returns the number of sectors reserved to EEPROM in the memory layout
 * @returns {uint8_t}           Number of sectors
 */
uint8_t EEPROM_Rotate::reserved() {
    uint32_t fs_end = (uint32_t)&_SPIFFS_end - 0x40200000;
    uint8_t sectors = (ESP.getFlashChipSize() - fs_end) / SPI_FLASH_SEC_SIZE - 4;
    return sectors;
}

/**
 * @brief Backups the current data to the given sector.
 * @param {uint32_t} target     Target sector (defaults to base sector)
 * @returns {bool}              True if seccessfully copied
 */
bool EEPROM_Rotate::backup(uint32_t target) {

    // Backup to the latest sector by default
    if (0 == target) target = base();

    // Do not backup if sector is already current
    if (_sector == target) return true;

    // Backup current index
    uint32_t backup_index = _sector_index;

    // Calculate new index (must be previous to target)
    _sector_index = _getIndex(target);
    if (0 == _sector_index) {
        _sector_index = _pool_size - 1;
    } else {
        --_sector_index;
    }

    // Flag as dirty to force write
    _dirty = true;

    // Do commit
    bool ret = commit();

    // If commit failed restore index
    if (!ret) {
        _sector_index = backup_index;
        _sector = _getSector(_sector_index);
    }

    return ret;

}

/**
 * @brief Dumps the EEPROM data to the given stream in a human-friendly way.
 * @param {Stream &}  debug     Stream to dump the data to
 * @param {uint32_t} sector     Sector to dump (default to current sector)
 */
void EEPROM_Rotate::dump(Stream & debug, uint32_t sector) {

    if (0 == sector) sector = _sector;
    if (sector > last() + 4) return;

    char ascii[17];
    memset(ascii, ' ', 16);
    ascii[16] = 0;

    debug.printf("\n         ");
    for (uint16_t i = 0; i <= 0x0F; i++) {
        debug.printf("%02X ", i);
    }
    debug.printf("\n------------------------------------------------------");

    uint8_t * data = new uint8_t[16];

    for (uint16_t address = 0; address < SPI_FLASH_SEC_SIZE; address++) {

        if ((address % 16) == 0) {
            noInterrupts();
            spi_flash_read(sector * SPI_FLASH_SEC_SIZE + address, reinterpret_cast<uint32_t*>(data), 16);
            interrupts();
            if (address > 0) {
                debug.print(ascii);
                memset(ascii, ' ', 16);
            }
            debug.printf("\n0x%04X:  ", address);
        }

        uint8_t b = data[address % 16];
        if (31 < b && b < 127) ascii[address % 16] = (char) b;
        debug.printf("%02X ", b);

        yield();

    }

    delete[] data;

    debug.print(ascii);
    debug.printf("\n\n");

}

// -----------------------------------------------------------------------------
// OVERWRITTEN METHODS
// -----------------------------------------------------------------------------

/**
 * @brief Loads 'size' bytes of data into memory for EEPROM emulation from the
 * latest valid sector in the sector pool
 * @param {size_t} size         Data size to read
 */
void EEPROM_Rotate::begin(size_t size) {

    uint32_t best_index = 0;
    uint8_t best_value = 0xFF;
    bool first = true;

    for (uint32_t index = 0; index < _pool_size; index++) {

        // load the sector data
        // Sector count goes downward,
        // so default EEPROM sector is always index 0
        _sector = _getSector(index);
        EEPROMClass::begin(size);

        // get sector value
        uint8_t value = read(_offset + EEPROM_ROTATE_COUNTER_OFFSET);
        DEBUG_EEPROM_ROTATE("Magic value for sector #%u is %u\n", _sector, value);

        // validate content
        if (!_checkCRC()) {
            DEBUG_EEPROM_ROTATE("Sector #%u has not passed the CRC check\n", _sector);
            continue;
        }

        // if this is the first valid sector we are reading
        if (first) {

            first = false;
            best_index = index;
            best_value = value;

        // else compare values
        } else {

            // This new sector is newer if...
            bool newer = false;
            uint8_t split = _pool_size - 1;
            if ((value < split) && (split < best_value)) {
                newer = true;
            } else if ((best_value < split) && (split < value)) {
                newer = false;
            } else {
                newer = value > best_value;
            }

            if (newer) {
                best_index = index;
                best_value = value;
            }

        }

    }

    // Re-read the data from the best index sector
    _sector_index = best_index;
    _sector = _getSector(_sector_index);
    EEPROMClass::begin(size);
    _sector_value = read(_offset + EEPROM_ROTATE_COUNTER_OFFSET);

    DEBUG_EEPROM_ROTATE("Current sector is #%u\n", _sector);
    DEBUG_EEPROM_ROTATE("Current magic value is #%u\n", _sector_value);

}

/**
 * @brief Writes data from memory to the next sector in the sector pool and flags it as current sector
 * @returns {bool}      True if successfully written
 */
bool EEPROM_Rotate::commit() {

    // Check if we are really going to write
    if (!_size) return false;
    if (!_dirty) return true;
    if (!_data) return false;

    // Backup current values
    uint8_t index_backup = _sector_index;
    uint8_t value_backup = _sector_value;

    // Update sector for next write
    _sector_index = (_sector_index + 1) % _pool_size;
    _sector = _getSector(_sector_index);
    _sector_value++;

    DEBUG_EEPROM_ROTATE("Writing to sector #%u\n", _sector);
    DEBUG_EEPROM_ROTATE("Writing magic value #%u\n", _sector_value);

    // Update the counter & crc bytes
    uint16_t crc = _calculateCRC();
    write(_offset + EEPROM_ROTATE_CRC_OFFSET, (crc >> 8) & 0xFF);
    write(_offset + EEPROM_ROTATE_CRC_OFFSET + 1, crc & 0xFF);
    write(_offset + EEPROM_ROTATE_COUNTER_OFFSET, _sector_value);

    // Perform the commit
    bool ret = EEPROMClass::commit();

    // If commit failed restore values
    if (!ret) {

        DEBUG_EEPROM_ROTATE("Commit to sector #%u failed, restoring\n", _sector);

        // Restore values
        _sector_index = index_backup;
        _sector_value = value_backup;
        _sector = _getSector(_sector_index);

    }

    return ret;

}

// -----------------------------------------------------------------------------
// PRIVATE METHODS
// -----------------------------------------------------------------------------

/**
 * @brief Calculates and automatically sets
 * the pool size based on the memory layout
 * @protected
 */
void EEPROM_Rotate::_auto() {
    uint8_t size = reserved();
    if (size > 10) size = 10;
    if (size < 1) size = 1;
    _pool_size = size;
}

/**
 * @brief Returns sector from index
 * @param {uint8_t} index       Sector index
 * @returns {uint32_t}          Sector number
 * @protected
 */
uint32_t EEPROM_Rotate::_getSector(uint8_t index) {
    return _base - index;
}

/**
 * @brief Returns index for sector
 * @param {uint32_t} sector     Sector number
 * @returns {uint8_t}           Sector index
 * @protected
 */
uint8_t EEPROM_Rotate::_getIndex(uint32_t sector) {
    return _base - sector;
}

/**
 * @brief Calculates the CRC of the data in memory (except for the magic bytes)
 * @returns {uint16_t}          CRC
 * @protected
 */
uint16_t EEPROM_Rotate::_calculateCRC() {
    uint16_t crc = 0;
    for (uint16_t address = 0; address < _size; address++) {
        if (_offset <= address && address <= _offset + 2) continue;
        crc = crc + read(address);
    }
    return crc;
}

/**
 * @brief Compares the CRC of the data in memory against the stored one
 * @returns {bool}              True if they match, so data is OK
 * @protected
 */
bool EEPROM_Rotate::_checkCRC() {
    uint16_t calculated = _calculateCRC();
    uint16_t stored =
        (read(_offset + EEPROM_ROTATE_CRC_OFFSET) << 8) +
        read(_offset + EEPROM_ROTATE_CRC_OFFSET + 1);
    DEBUG_EEPROM_ROTATE("Calculated CRC: 0x%04X\n", calculated);
    DEBUG_EEPROM_ROTATE("Stored CRC    : 0x%04X\n", stored);
    return (calculated == stored);
}
