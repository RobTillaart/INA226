
[![Arduino CI](https://github.com/RobTillaart/INA226/workflows/Arduino%20CI/badge.svg)](https://github.com/marketplace/actions/arduino_ci)
[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/RobTillaart/INA226/blob/master/LICENSE)
[![GitHub release](https://img.shields.io/github/release/RobTillaart/INA226.svg?maxAge=3600)](https://github.com/RobTillaart/INA226/releases)


# INA226

Arduino library for the INA226 power sensor


## Description



## Interface

- **INA226(const int8_t address, TwoWire \*wire = Wire)** Constructor to set address and optional Wire interface.
- **bool begin(const uint8_t sda, const uint8_t scl)** for ESP32 and ESP8266;  initializes the class.
sets I2C pins.
returns true if the INA226 address is on the I2C bus.
- **bool begin()** UNO ea. initializes the class. 
returns true if the LTC2991 address is on the I2C bus.
- **bool isConnected()** returns true if the LTC2991 address is on the I2C bus.




## Operational

See examples..


## TODO


