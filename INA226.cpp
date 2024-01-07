//    FILE: INA226.cpp
//  AUTHOR: Rob Tillaart
// VERSION: 0.5.2
//    DATE: 2021-05-18
// PURPOSE: Arduino library for INA226 power sensor
//     URL: https://github.com/RobTillaart/INA226


#include "INA226.h"

//  REGISTERS
#define INA226_CONFIGURATION            ( 0x00 )
#define INA226_SHUNT_VOLTAGE            ( 0x01 )
#define INA226_BUS_VOLTAGE              ( 0x02 )
#define INA226_POWER                    ( 0x03 )
#define INA226_CURRENT                  ( 0x04 )
#define INA226_CALIBRATION              ( 0x05 )
#define INA226_MASK_ENABLE              ( 0x06 )
#define INA226_ALERT_LIMIT              ( 0x07 )
#define INA226_MANUFACTURER             ( 0xFE )
#define INA226_DIE_ID                   ( 0xFF )

//  CONFIGURATION MASKS
#define INA226_CONF_RESET_MASK          ( 0x8000 )
#define INA226_CONF_AVERAGE_MASK        ( 0x0E00 )
#define INA226_CONF_BUSVC_MASK          ( 0x01C0 )
#define INA226_CONF_SHUNTVC_MASK        ( 0x0038 )
#define INA226_CONF_MODE_MASK           ( 0x0007 )

//  set by setAlertRegister
#define INA226_SHUNT_OVER_VOLTAGE       ( 0x8000 )
#define INA226_SHUNT_UNDER_VOLTAGE      ( 0x4000 )
#define INA226_BUS_OVER_VOLTAGE         ( 0x2000 )
#define INA226_BUS_UNDER_VOLTAGE        ( 0x1000 )
#define INA226_POWER_OVER_LIMIT         ( 0x0800 )
#define INA226_CONVERSION_READY         ( 0x0400 )

//  returned by getAlertFlag
#define INA226_ALERT_FUNCTION_FLAG      ( 0x0010 )
#define INA226_CONVERSION_READY_FLAG    ( 0x0008 )
#define INA226_MATH_OVERFLOW_FLAG       ( 0x0004 )
#define INA226_ALERT_POLARITY_FLAG      ( 0x0002 )
#define INA226_ALERT_LATCH_ENABLE_FLAG  ( 0x0001 )

//  returned by setMaxCurrentShunt
#define INA226_ERR_NONE                 ( 0x0000 )
#define INA226_ERR_SHUNTVOLTAGE_HIGH    ( 0x8000 )
#define INA226_ERR_MAXCURRENT_LOW       ( 0x8001 )
#define INA226_ERR_SHUNT_LOW            ( 0x8002 )
#define INA226_ERR_NORMALIZE_FAILED     ( 0x8003 )
#define INA226_ERR_WRITE_REG_FAILED     ( 0x8004 )


//  AVG SETTING TO VALUE CONVERSION ARRAY
const uint16_t INA226_AVERAGE[] = {
    1,      // INA226_NO_AVERAGE
    4,      // INA226_AVERAGE_4_SAMPLES
    16,     // INA226_AVERAGE_16_SAMPLES
    64,     // INA226_AVERAGE_64_SAMPLES
    128,    // INA226_AVERAGE_128_SAMPLES
    256,    // INA226_AVERAGE_256_SAMPLES
    512,    // INA226_AVERAGE_512_SAMPLES
    1024    // INA226_AVERAGE_1024_SAMPLES
    };

//  VBUS & VSHunt CONVERSION TIME SETTING TO VALUE CONVERSION ARRAY
const uint16_t INA226_CT_US[] = {
    140,    // INA226_CT_SETTING_140_US
    204,    // INA226_CT_SETTING_204_US
    332,    // INA226_CT_SETTING_332_US
    588,    // INA226_CT_SETTING_588_US
    1100,   // INA226_CT_SETTING_1100_US
    2116,   // INA226_CT_SETTING_2116_US
    4156,   // INA226_CT_SETTING_4156_US
    8244    // INA226_CT_SETTING_8244_US
    };

//  MODE SETTING TO VBUS MEASUREMENT OR NOT MAPPING ARRAY
const bool INA226_VBUS_MEASUREMENT[] = {
    false,  // INA226_POWERDOWN_MODE / INA226_SHUTDOWN_MODE
    false,  // INA226_SHUNTTRIGGER_MODE
    true,   // INA226_BUSTRIGGER_MODE
    true,   // INA226_SHUNTBUSTRIGGER_MODE
    false,  // "INA226_POWERDOWN_MODE / INA226_SHUTDOWN_MODE"
    false,  // INA226_SHUNTCONTINUOUS_MODE
    true,   // INA226_BUSCONTINUOUS_MODE
    true    // INA226_SHUNTBUSCONTINUOUS_MODE
    };

//  MODE SETTING TO VSHUNT MEASUREMENT OR NOT MAPPING ARRAY
const bool INA226_VSH_MEASUREMENT[] = {
    false,  // INA226_POWERDOWN_MODE / INA226_SHUTDOWN_MODE
    true,   // INA226_SHUNTTRIGGER_MODE
    false,  // INA226_BUSTRIGGER_MODE
    true,   // INA226_SHUNTBUSTRIGGER_MODE
    false,  // "INA226_POWERDOWN_MODE / INA226_SHUTDOWN_MODE"
    true,   // INA226_SHUNTCONTINUOUS_MODE
    false,  // INA226_BUSCONTINUOUS_MODE
    true    // INA226_SHUNTBUSCONTINUOUS_MODE
    };


////////////////////////////////////////////////////////
//
//  Constructor
//
INA226::INA226(const uint8_t address, TwoWire *wire)
{
  _address     = address;
  _wire        = wire;
  //  not calibrated values by default.
  _current_LSB = 0;
  _maxCurrent  = 0;
  _shunt       = 0;
  //  Default mode, conversion time and average (see Datasheet)
  _mode        = INA226_SHUNTBUSCONTINUOUS_MODE;
  _avg         = INA226_NO_AVERAGE;
  _bvct        = INA226_CT_SETTING_1100_US;
  _svct        = INA226_CT_SETTING_1100_US;
}


bool INA226::begin()
{
  if (! isConnected()) return false;
  return true;
}


bool INA226::isConnected()
{
  _wire->beginTransmission(_address);
  return ( _wire->endTransmission() == 0);
}


uint8_t INA226::getAddress()
{
  return _address;
};


////////////////////////////////////////////////////////
//
//  Core functions
//
float INA226::getBusVoltage()
{
  uint16_t val = _readRegister(INA226_BUS_VOLTAGE);
  return val * 1.25e-3;  //  fixed 1.25 mV
}


float INA226::getShuntVoltage()
{
  int16_t val = _readRegister(INA226_SHUNT_VOLTAGE);
  return val * 2.5e-6;   //  fixed 2.50 uV
}


float INA226::getCurrent()
{
  int16_t val = _readRegister(INA226_CURRENT);
  return val * _current_LSB;
}


float INA226::getPower()
{
  uint16_t val = _readRegister(INA226_POWER);
  return val * 25 * _current_LSB;  //  fixed 25 Watt
}


bool INA226::isConversionReady()
{
  uint16_t mask = _readRegister(INA226_MASK_ENABLE);
  return (mask & INA226_CONVERSION_READY_FLAG) == INA226_CONVERSION_READY_FLAG;
}


bool INA226::waitConversionReady()
{
  // Quick return if Ready (will be most of the times)
  if (isConversionReady()) return true;

  uint16_t dSamples = INA226_AVERAGE[_avg];
  uint32_t dBvct_us = INA226_VBUS_MEASUREMENT[_mode] * dSamples * INA226_CT_US[_bvct];
  uint32_t dSvct_us = INA226_VSH_MEASUREMENT[_mode] * dSamples * INA226_CT_US[_svct];
  // There seems to be an almost fixed factor of 1.03 between theoretical and measured CT
  // Let's take a factor of 1.05 just to have some slack and add 2 because of 2x rounding
  uint32_t timeout_ms = ((105 * dBvct_us) / 100000) + ((105 * dSvct_us) / 100000) + 2;

  uint32_t start = millis();
  while ((millis() - start) <= timeout_ms) 
  {
    if (isConversionReady()) return true;
    delay(1);
  }

  return false;
}


bool INA226::waitConversionReady(uint32_t timeout_ms)
{
  // Quick return if Ready (will be most of the times)
  if (isConversionReady()) return true;

  uint32_t start = millis();
  while ((millis() - start) <= timeout_ms) 
  {
    if (isConversionReady()) return true;
    delay(1);
  }
  return false;
}

////////////////////////////////////////////////////////
//
//  Configuration
//
bool INA226::reset()
{
  uint16_t mask = _readRegister(INA226_CONFIGURATION);
  mask |= INA226_CONF_RESET_MASK;
  if (_writeRegister(INA226_CONFIGURATION, mask)) return false;
  //  reset calibration
  _current_LSB = 0;
  _maxCurrent  = 0;
  _shunt       = 0;
  //  Default mode, conversion time and average
  _mode        = INA226_SHUNTBUSCONTINUOUS_MODE;
  _avg         = INA226_NO_AVERAGE;
  _bvct        = INA226_CT_SETTING_1100_US;
  _svct        = INA226_CT_SETTING_1100_US;
  return true;
}


bool INA226::setAverage(ina226_avg_enum avg)
{
  uint16_t mask = _readRegister(INA226_CONFIGURATION);
  mask &= ~INA226_CONF_AVERAGE_MASK;
  mask |= (avg << 9);
  if (_writeRegister(INA226_CONFIGURATION, mask)) return false;
  _avg = avg;
  return true;
}


ina226_avg_enum INA226::getAverage()
{
  uint16_t mask = _readRegister(INA226_CONFIGURATION);
  mask &= INA226_CONF_AVERAGE_MASK;
  mask >>= 9;
  _avg = static_cast<ina226_avg_enum>(mask);
  return _avg;
}


bool INA226::setBusVoltageConversionTime(ina226_ct_enum bvct)
{
  uint16_t mask = _readRegister(INA226_CONFIGURATION);
  mask &= ~INA226_CONF_BUSVC_MASK;
  mask |= (bvct << 6);
  if (_writeRegister(INA226_CONFIGURATION, mask)) return false;
  _bvct = bvct;
  return true;
}


ina226_ct_enum INA226::getBusVoltageConversionTime()
{
  uint16_t mask = _readRegister(INA226_CONFIGURATION);
  mask &= INA226_CONF_BUSVC_MASK;
  mask >>= 6;
  _bvct = static_cast<ina226_ct_enum>(mask);
  return _bvct;
}


bool INA226::setShuntVoltageConversionTime(ina226_ct_enum svct)
{
  uint16_t mask = _readRegister(INA226_CONFIGURATION);
  mask &= ~INA226_CONF_SHUNTVC_MASK;
  mask |= (svct << 3);
  if (_writeRegister(INA226_CONFIGURATION, mask)) return false;
  _svct = svct;
  return true;
}


ina226_ct_enum INA226::getShuntVoltageConversionTime()
{
  uint16_t mask = _readRegister(INA226_CONFIGURATION);
  mask &= INA226_CONF_SHUNTVC_MASK;
  mask >>= 3;
  _svct = static_cast<ina226_ct_enum>(mask);
  return _svct;
}


////////////////////////////////////////////////////////
//
//  Calibration
//
int INA226::setMaxCurrentShunt(float maxCurrent, float shunt, bool normalize)
{
  //  https://github.com/RobTillaart/INA226/pull/29

  //  #define printdebug true

  //  fix #16 - datasheet 6.5 Electrical Characteristics
  //            rounded value to 80 mV
  float shuntVoltage = maxCurrent * shunt;
  if (shuntVoltage > 0.080)         return INA226_ERR_SHUNTVOLTAGE_HIGH;
  if (maxCurrent < 0.001)           return INA226_ERR_MAXCURRENT_LOW;
  if (shunt < INA226_MINIMAL_SHUNT) return INA226_ERR_SHUNT_LOW;

  _current_LSB = maxCurrent * 3.0517578125e-5;      //  maxCurrent / 32768;

  #ifdef printdebug
    Serial.println();
    Serial.print("normalize:\t");
    Serial.println(normalize ? " true":" false");
    Serial.print("initial current_LSB:\t");
    Serial.print(_current_LSB * 1e+6, 1);
    Serial.println(" uA / bit");
  #endif

  //  normalize the LSB to a round number
  //  LSB will increase
  if (normalize)
  {
    /*
       check if maxCurrent (normal) or shunt resistor 
       (due to unusual low resistor values in relation to maxCurrent) determines currentLSB
       we have to take the upper value for currentLSB
    
       calculation of currentLSB based on shunt resistor and calibration register limits (2 bytes)
       cal = 0.00512 / ( shunt * currentLSB )
       cal(max) = 2^16-1 
       currentLSB(min) = 0.00512 / ( shunt * cal(max) )
       currentLSB(min) ~= 0.00512 / ( shunt * 2^16 )
       currentLSB(min) ~= 2^9 * 1e-5 / ( shunt * 2^16 )  
       currentLSB(min) ~= 1e-5 / 2^7 / shunt
       currentLSB(min) ~= 7.8125e-8 / shunt 
    */
    if ( 7.8125e-8 / shunt > _current_LSB ) {
      // shunt resistor determines currentLSB -> take this a starting point for currentLSB
      _current_LSB = 7.8125e-8 / shunt;
    }

    #ifdef printdebug
      Serial.print("Prescale current_LSB:\t");
      Serial.print(_current_LSB * 1e+6, 1);
      Serial.println(" uA / bit");
    #endif

    //  normalize _current_LSB to a value of 1, 2 or 5 * 1e-6 to 1e-3
    //  convert float to int
    uint16_t currentLSB_uA = float(_current_LSB * 1e+6);
    currentLSB_uA++;  //  ceil() would be more precise, but uses 176 bytes of flash.

    uint16_t factor = 1;  //  1uA to 1000uA
    uint8_t i = 0;        //  1 byte loop reduces footprint
    bool result = false;
    do {
      if ( 1 * factor >= currentLSB_uA) {
        _current_LSB = 1 * factor * 1e-6;
        result = true;
      } else if ( 2 * factor >= currentLSB_uA) {
        _current_LSB = 2 * factor * 1e-6;
        result = true;
      } else if ( 5 * factor >= currentLSB_uA) {
        _current_LSB = 5 * factor * 1e-6;
        result = true;
      } else {
        factor *= 10;
        i++;
      }
    } while( (i < 4) && (!result) );  //  factor < 10000 

    if (result == false) {  //  not succeeded to normalize.
      _current_LSB = 0;
      return INA226_ERR_NORMALIZE_FAILED;
    }
  
    #ifdef printdebug
      Serial.print("After scale current_LSB:\t");
      Serial.print(_current_LSB * 1e+6, 1);
      Serial.println(" uA / bit");
    #endif
    // done
  }

  //  auto scale calibration if needed.
  uint32_t calib = round(0.00512 / (_current_LSB * shunt));
  while (calib > 65535)
  {
    _current_LSB *= 2;
    calib >>= 1;
  }
  if (_writeRegister(INA226_CALIBRATION, calib)) return INA226_ERR_WRITE_REG_FAILED;

  _maxCurrent = _current_LSB * 32768;
  _shunt = shunt;

  #ifdef printdebug
    Serial.print("Final current_LSB:\t");
    Serial.print(_current_LSB * 1e+6, 1);
    Serial.println(" uA / bit");
    Serial.print("Calibration:\t");
    Serial.println(calib);
    Serial.print("Max current:\t");
    Serial.print(_maxCurrent, 3);
    Serial.println(" A");
    Serial.print("Shunt:\t");
    Serial.print(_shunt, 4);
    Serial.println(" Ohm");
    Serial.print("ShuntV:\t");
    Serial.print(shuntVoltage, 4);
    Serial.println(" Volt");
  #endif

  return INA226_ERR_NONE;
}


////////////////////////////////////////////////////////
//
//  operating mode
//
bool INA226::setMode(ina226_mode_enum mode)
{
  uint16_t config = _readRegister(INA226_CONFIGURATION);
  config &= ~INA226_CONF_MODE_MASK;
  config |= mode;
  if (_writeRegister(INA226_CONFIGURATION, config)) return false;
  _mode = mode;
  return true;
}


ina226_mode_enum INA226::getMode()
{
  uint16_t mode = _readRegister(INA226_CONFIGURATION);
  mode &= INA226_CONF_MODE_MASK;
  _mode = static_cast<ina226_mode_enum>(mode);
  return _mode;
}


////////////////////////////////////////////////////////
//
//  alert
//
bool INA226::setAlertRegister(uint16_t mask)
{
  if (_writeRegister(INA226_MASK_ENABLE, (mask & 0xFC00))) return false;
  return true;
}


uint16_t INA226::getAlertFlag()
{
  return _readRegister(INA226_MASK_ENABLE) & 0x001F;
}


bool INA226::setAlertLimit(uint16_t limit)
{
  if (_writeRegister(INA226_ALERT_LIMIT, limit)) return false;
  return true;
}


uint16_t INA226::getAlertLimit()
{
  return _readRegister(INA226_ALERT_LIMIT);
}


////////////////////////////////////////////////////////
//
//  meta information
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
//  PRIVATE
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


//  -- END OF FILE --

