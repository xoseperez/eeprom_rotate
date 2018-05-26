# EEPROM Rotate

This is a wrapper around the Arduino Core for ESP8266 EEPROM library that handles sector rotating while keeping full compatibility with the original API.

[![version](https://img.shields.io/badge/version-0.1.0
-brightgreen.svg)](CHANGELOG.md)
[![travis](https://travis-ci.org/xoseperez/eeprom_rotate.git.svg?branch=master)](https://travis-ci.org/xoseperez/eeprom_rotate.git)
[![codacy](https://img.shields.io/codacy/grade/2f06a871848345368445ea1b74796f4c/master.svg)](https://www.codacy.com/app/xoseperez/eeprom_rotate.git/dashboard)
[![license](https://img.shields.io/github/license/xoseperez/eeprom_rotate.svg)](LICENSE)
<br />
[![donate](https://img.shields.io/badge/donate-PayPal-blue.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=xose%2eperez%40gmail%2ecom&lc=US&no_note=0&currency_code=EUR&bn=PP%2dDonationsBF%3abtn_donate_LG%2egif%3aNonHostedGuest)
[![twitter](https://img.shields.io/twitter/follow/xoseperez.svg?style=social)](https://twitter.com/intent/follow?screen_name=xoseperez)

The emulated EEPROM in the ESP8266 uses one SPI flash memory sector to store the data. Due to the nature of this flash memory (NOR) a full sector erase must be done prior to write any new data. If a power failure (intended or not) happens during this process the sector data is lost.

Also, writing data to a NOR memory can be done byte by byte but only to change a 1 to a 0. The only way to turn 0s to 1s is to perform a sector erase which turns all memory positions in that sector to 1. But sector erasing must be done in full sectors, thus wearing out the flash memory faster.

A way to overcome this is to use more than one sector to store data and check on boot which one is has the latest valid data.

This is what this library does.

## How does it work

The library overwrites two methods of the original one: `begin` and `commit`.

The `begin` method will load the data from all the available sectors one after the other trying to figure out which one has the **latest valid information**. To do
this it checks two values:

* A 2-bytes CRC
* A 1-byte auto-increment number

These values are stored in a certain position in the sector (at the very beginning by default but the user can choose another position with the `offset` method).

The CRC is calculated based on the contents of the sector (except for those special 3 bytes). If the calculated CRC matches that stored in the sector then the library checks the auto-increment and selects the sector with the most recent number (taking overflows into account, of course).

Those special values are stored by the overwritten `commit` method prior to the
actual commit.

After every successful commit, the library will hop to the next sector for the next commit. This way, in case of a power failure in the middle of a commit, the  CRC for that sector will fail and the library will use the data in the latest known-good sector.

## API

The library inherits form the Arduino Core for ESP8266 EEPROM library, and it shares the same API. You can just replace one with the other. The same public methods with the same signature.

By default the library uses the same default sector as the original (the fifth sector counting from the end) and just that sector. So it will behave just like the original.

The initial sector will be the same as with the EEPROM library (sector 1019 for 4Mb boards, sector 251 for 1Mb boards). The other sectors are the ones counting from the initial one downwards. This means that if we set up a sector rotation of 4 for a 4Mb board using default initial sector, the used sectors will be 1019, 1018, 1017 and 1016.

You must take this into account since it reduces the available size for program memory and OTA updates.

The library exposes a set of new methods to configure the sector rotating and performing other special actions:

#### void rotate(uint8_t sectors)

Set the number of sectors that the library will use. The default value is 1. It must be called before the `begin` method. Sectors are indexed counting downwards.

#### void offset(uint8_t offset)

Define the offset in the sector where the special auto-increment and CRC values will be stores. The default value is 0. This special data uses 3 bytes of space in the EEPROM.

#### uint8_t last()

Returns the number of the last available sector for EEPROM. You can use this value to choose a sensible rotation size:

```
uint8_t size = 0;
if (EEPROM.last() > 1000) { // 4Mb boards
    size = 4;
} else if (EEPROM.last() > 250) { // 1Mb boards
    size = 2;
} else {
    size = 1;
}
EEPROM.rotate(size);
EEPROM.begin();
```

#### bool erase(uint32_t sector)

Erases the given sector. Use with caution.

#### bool eraseAll()

Erases all the sections in the rotation pool. Use with caution.

#### void dump(Stream & debug)

Dumps the EEPROM data to the given stream in a human-friendly way.

## Notes

### Disabling the original global EEPROM object

The original EEPROM library automatically instantiates an EEPROM object that's
already available to use. This consumes little memory (since the data buffer is
only created and populated when calling `begin`). But anyway if you don't want to
have a unused object around you can disable the object instantiation by adding
the `NO_GLOBAL_EEPROM` build flag.

## License

Copyright (C) 2018 by Xose Pérez (@xoseperez)

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
