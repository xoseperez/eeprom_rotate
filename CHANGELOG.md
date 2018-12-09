# EEPROM_Rotate change log

The format is based on [Keep a Changelog](http://keepachangelog.com/)
and this project adheres to [Semantic Versioning](http://semver.org/).

## [0.9.2] 2018-12-09
### Fix
- Fix magic number check around split (thanks to @arihantdaga and @mcspr)

## [0.9.1] 2018-06-12
### Added
- Added `rotate` method to enable/disable sector rotation

## [0.9.0] 2018-06-03
### Changed
- Renamed `pool` as `size`
- Deleted `erase` and `eraseAll` methods

### Fixed
- Fix offset variable size

## [0.1.1] 2018-06-02
### Added
- Examples on how to use custom memory layouts
- Auto-discover EEPROM pool size based on memory layout
- Added yield to dump loop
- OTA example

## [0.1.0] 2018-05-28
- Initial working version
