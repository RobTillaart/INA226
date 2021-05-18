
[![Arduino CI](https://github.com/RobTillaart/INA226/workflows/Arduino%20CI/badge.svg)](https://github.com/marketplace/actions/arduino_ci)
[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/RobTillaart/INA226/blob/master/LICENSE)
[![GitHub release](https://img.shields.io/github/release/RobTillaart/INA226.svg?maxAge=3600)](https://github.com/RobTillaart/INA226/releases)


# INA226

Arduino library for the INA226 power sensor


## Description

Experimental library for the ONA226 power sensor.
Not all functionality is tested / investigated.

The INA226 is a voltage, current and power measurement device.
- max voltage 36 Volt
- max current ?? Ampere

(to elaborate).



## Interface

read datasheet for details

### Constructor

- **INA226(const int8_t address, TwoWire \*wire = Wire)** Constructor to set address and optional Wire interface.
- **bool begin(const uint8_t sda, const uint8_t scl)** for ESP32 and ESP8266;  initializes the class.
sets I2C pins.
returns true if the INA226 address is on the I2C bus.
- **bool begin()** UNO ea. initializes the class. 
returns true if the LTC2991 address is on the I2C bus.
- **bool isConnected()** returns true if the LTC2991 address is on the I2C bus.


### Core Functions

Note the power and the current are not meaningful without calibrating
the sensor. Also the value is not meaningful if there is no shunt connected.

- **float getShuntVoltage()** idem.
- **float getBusVoltage()** idem. Max 36 Volt.
- **float getPower()** is Current x BusVoltage
- **float getCurrent()** is Current through the shunt.


### Configuration

to be tested.

- **void reset()** software power on reset
- **void setAverage(uint8_t avg = 0)** see table datasheet
(0 = default ==> 1 read)
- **uint8_t getAverage()** return the value set. Note this is not the count of samples.
- **void setBusVoltageConversionTime(uint8_t bvct = 4)** see table datasheet
(4 = default ==> 1.1 ms)
- **uint8_t  getBusVoltageConversionTime()** return the value set. Note this is not a unit of time.
- **void setShuntVoltageConversionTime(uint8_t svct = 4)** see table datasheet
(4 = default ==> 1.1 ms)
- **uint8_t getShuntVoltageConversionTime()** return the value set. Note this is not a unit of time.

### Calibration

- **void setMaxCurrentShunt(float ampere = 10.0, float ohm = 0.1)** set the calibration register based upon the shunt and the max ampere. From this the LSB is derived. Note the function will round up the LSB to nearest round value.
- **float getCurrentLSB()** returns the LSB == precission of the calibration


### Operating mode

See datasheet, to be tested

- **void setMode(uint8_t mode = 7)** mode = 0.. 7
- **void shutDown()**
- **void setModeShuntTrigger()** how to trigger to be investigated
- **void setModeBusTrigger()** 
- **void setModeShuntBusTrigger()**
- **void setModeShuntContinuous()** 
- **void setModeBusContinuous()**
- **void setModeShuntBusContinuous()** Default mode, works well
- **uint8_t getMode()** returns the mode set.


### Alert functions

See datasheet, to be tested.

- **void setAlertRegister(uint16_t mask)** an over or under flow can be detected
- **uint16_t getAlertFlag()** returns the mask set
- **void setAlertLimit(uint16_t limit)** sets the limit that belongs to the Alert Flag
- **uint16_t getAlertLimit()** return the set limits


### Meta information

- **uint16_t getManufacturerID()** s return 0x5449
- **uint16_t getDieID()** should return 0x2260


## Operational

See examples..


## TODO

- testtestestest
- improve readme.md
- add tables to readme.md
