//    FILE: INA266.h
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.0
//    DATE: 2021-05-18
// PURPOSE: Arduino library for INA266 power sensor
//     URL: https://github.com/RobTillaart/INA226
//
//  HISTORY:
//  0.1.0   2021-05-18  initial version


#include "INA226.h"

#define INA226_CONFIGURATION        0x00
#define INA226_SHUNT_VOLTAGE        0x01
#define INA226_BUS_VOLTAGE          0x02
#define INA226_POWER                0x03
#define INA226_CURRENT              0x04
#define INA226_CALIBRATION          0x05
#define INA226_MASK_ENABLE          0x06
#define INA226_MANUFACTURER         0xFE
#define INA226_DIE_ID               0xFF


////////////////////////////////////////////////////////
//
// Constructor
//
INA226::INA226(const int8_t address, TwoWire *wire)
{
  // address should be between 0x40 and 0x4F
  _address = address;
  _wire = wire;
}


#if defined (ESP8266) || defined(ESP32)
bool INA226::begin(const uint8_t sda, const uint8_t scl)
{
  _wire = &Wire;
  _wire->begin(sda, scl);
  if (! isConnected()) return false;
  return true;
}
#endif


bool INA226::begin()
{
  _wire->begin();
  if (! isConnected()) return false;
  return true;
}


bool INA226::isConnected()
{
  _wire->beginTransmission(_address);
  return ( _wire->endTransmission() == 0);
}


////////////////////////////////////////////////////////
//
// Core functions
//
float INA226::getShuntVoltage()
{
  uint16_t val = _readRegister(INA226_SHUNT_VOLTAGE);
  if (val & 0x8000)
  {
    val = val & 0x7FFF;
    val = val ^ 0x7FFF;
    val++;
    return val * -2.5e-6;
  }
  return val * 2.5e-6;
}


float INA226::getBusVoltage()
{
  uint16_t val = _readRegister(INA226_BUS_VOLTAGE);
  return val * 1.25e-3;
}


float INA226::getPower()
{
  // TODO
  uint16_t val = _readRegister(INA226_POWER);
  return val * 1.0;
}


float INA226::getCurrent()
{
  // TODO
  uint16_t val = _readRegister(INA226_CURRENT);
  return val * 1.0;
}


////////////////////////////////////////////////////////
//
// calibration & configuration
//
void INA226::reset()
{
  uint16_t m = _readRegister(INA226_CONFIGURATION);
  m |= 0x800;
  _writeRegister(INA226_CONFIGURATION, m);
}

void INA226::setAverage(uint8_t avg)
{
  if (avg > 7) return;
  uint16_t m = _readRegister(INA226_CONFIGURATION);
  m &= 0xF1FF;
  m |= (avg << 9);
  _writeRegister(INA226_CONFIGURATION, m);
}


void INA226::setBusVoltageConversionTime(uint8_t bvct)
{
  if (bvct > 7) return;
  uint16_t m = _readRegister(INA226_CONFIGURATION);
  m &= 0xFE3F;
  m |= (bvct << 6);
  _writeRegister(INA226_CONFIGURATION, m);
}


void INA226::setShuntVoltageConversionTime(uint8_t svct)
{
  if (svct > 7) return;
  uint16_t m = _readRegister(INA226_CONFIGURATION);
  m &= 0xFFC7;
  m |= (svct << 3);
  _writeRegister(INA226_CONFIGURATION, m);
}


////////////////////////////////////////////////////////
//
// operating mode
//
void INA226::setMode(uint8_t mode)
{
  if (mode > 7) return;
  uint16_t m = _readRegister(INA226_CONFIGURATION);
  m &= 0xFFF8;
  m |= mode;
  _writeRegister(INA226_CONFIGURATION, m);
}


uint8_t INA226::getMode()
{
  return _readRegister(INA226_CONFIGURATION) & 0x0007;
}


////////////////////////////////////////////////////////
//
// meta information
//
uint16_t INA226::getManufacturerID()
{
  return _readRegister(INA226_MANUFACTURER);
}

uint16_t INA226::getDieID()
{
  return _readRegister(INA226_DIE_ID);
}


////////////////////////////////////////////////////////
//
// PRIVATE
//
uint16_t INA226::_readRegister(uint8_t reg)
{
  _wire->beginTransmission(_address);
  _wire->write(reg);
  _wire->endTransmission();

  _wire->requestFrom(_address, (uint8_t)2);
  uint16_t value = _wire->read();
  value <<= 8;
  value |= _wire->read();
  return value;
}


uint16_t INA226::_writeRegister(uint8_t reg, uint16_t value)
{
  _wire->beginTransmission(_address);
  _wire->write(reg);
  _wire->write(value >> 8);
  _wire->write(value & 0xFF);
  return _wire->endTransmission();
}


// -- END OF FILE --

