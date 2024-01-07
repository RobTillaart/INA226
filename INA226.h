#pragma once
//    FILE: INA226.h
//  AUTHOR: Rob Tillaart
// VERSION: 0.5.2
//    DATE: 2021-05-18
// PURPOSE: Arduino library for INA226 power sensor
//     URL: https://github.com/RobTillaart/INA226
//
//  Read the datasheet for the details
//


#include "Arduino.h"
#include "Wire.h"


#define INA226_LIB_VERSION      ( "0.5.2" )


//  See issue #26
#define INA226_MINIMAL_SHUNT    ( 0.001 )

//  AVERAGE (AVG) SETTINGS
enum ina226_avg_enum {
    INA226_NO_AVERAGE           = 0,
    INA226_AVERAGE_4_SAMPLES    = 1,
    INA226_AVERAGE_16_SAMPLES   = 2,
    INA226_AVERAGE_64_SAMPLES   = 3,
    INA226_AVERAGE_128_SAMPLES  = 4,
    INA226_AVERAGE_256_SAMPLES  = 5,
    INA226_AVERAGE_512_SAMPLES  = 6,
    INA226_AVERAGE_1024_SAMPLES = 7
};


//  VBUS & VSHunt CONVERSION TIME (CT) SETTINGS
enum ina226_ct_enum {
    INA226_CT_SETTING_140_US  = 0,
    INA226_CT_SETTING_204_US  = 1,
    INA226_CT_SETTING_332_US  = 2,
    INA226_CT_SETTING_588_US  = 3,
    INA226_CT_SETTING_1100_US = 4,
    INA226_CT_SETTING_2116_US = 5,
    INA226_CT_SETTING_4156_US = 6,
    INA226_CT_SETTING_8244_US = 7
};


// MODE SETTINGS
// Note : 0 and 4 value have the same function
enum ina226_mode_enum {
    INA226_SHUTDOWN_MODE           = 0,
    INA226_SHUNTTRIGGER_MODE       = 1,
    INA226_BUSTRIGGER_MODE         = 2,
    INA226_SHUNTBUSTRIGGER_MODE    = 3,
    INA226_SHUNTCONTINUOUS_MODE    = 5,
    INA226_BUSCONTINUOUS_MODE      = 6,
    INA226_SHUNTBUSCONTINUOUS_MODE = 7
};


class INA226
{
public:
  //  address between 0x40 and 0x4F
  explicit INA226(const uint8_t address, TwoWire *wire = &Wire);

  bool     begin();
  bool     isConnected();
  uint8_t  getAddress();


  //  Core functions
  float    getBusVoltage();                             //  Volt
  float    getShuntVoltage();                           //  Volt
  float    getCurrent();                                //  Ampere
  float    getPower();                                  //  Watt
  bool     isConversionReady();                         //  Conversion ready flag is set
  bool     waitConversionReady();                       //  Calculated timed-out wait for ready
  bool     waitConversionReady(uint32_t timeout_ms);    //  Given timed-out wait for ready


  //  Scale helpers milli range
  float    getBusVoltage_mV()   { return getBusVoltage()   * 1e3; };
  float    getShuntVoltage_mV() { return getShuntVoltage() * 1e3; };
  float    getCurrent_mA()      { return getCurrent()      * 1e3; };
  float    getPower_mW()        { return getPower()        * 1e3; };
  //  Scale helpers micro range
  float    getBusVoltage_uV()   { return getBusVoltage()   * 1e6; };
  float    getShuntVoltage_uV() { return getShuntVoltage() * 1e6; };
  float    getCurrent_uA()      { return getCurrent()      * 1e6; };
  float    getPower_uW()        { return getPower()        * 1e6; };


  //  Configuration
  bool            reset();
  bool            setAverage(ina226_avg_enum avg = INA226_NO_AVERAGE);
  ina226_avg_enum getAverage();
  bool            setBusVoltageConversionTime(ina226_ct_enum bvct = INA226_CT_SETTING_1100_US);
  ina226_ct_enum  getBusVoltageConversionTime();
  bool            setShuntVoltageConversionTime(ina226_ct_enum svct = INA226_CT_SETTING_1100_US);
  ina226_ct_enum  getShuntVoltageConversionTime();


  //  Calibration
  //  mandatory to set these!
  //  shunt * maxCurrent < 80 mV
  //  maxCurrent >= 0.001
  //  shunt      >= 0.001
  int      setMaxCurrentShunt(float maxCurrent = 20.0, float shunt = 0.002, bool normalize = true);
  bool     isCalibrated() { return _current_LSB != 0.0; };

  //  These functions return zero if not calibrated!
  float    getCurrentLSB()    { return _current_LSB;       };
  float    getCurrentLSB_mA() { return _current_LSB * 1e3; };
  float    getCurrentLSB_uA() { return _current_LSB * 1e6; };
  float    getShunt()         { return _shunt;             };
  float    getMaxCurrent()    { return _maxCurrent;        };


  //  Operating mode
  bool             setMode(ina226_mode_enum mode = INA226_SHUNTBUSCONTINUOUS_MODE);
  ina226_mode_enum getMode();
  bool             shutDown()                  { return setMode(INA226_SHUTDOWN_MODE);           };
  bool             setModeShuntTrigger()       { return setMode(INA226_SHUNTTRIGGER_MODE);       };
  bool             setModeBusTrigger()         { return setMode(INA226_BUSTRIGGER_MODE);         };
  bool             setModeShuntBusTrigger()    { return setMode(INA226_SHUNTBUSTRIGGER_MODE);    };
  bool             setModeShuntContinuous()    { return setMode(INA226_SHUNTCONTINUOUS_MODE);    };
  bool             setModeBusContinuous()      { return setMode(INA226_BUSCONTINUOUS_MODE);      };
  bool             setModeShuntBusContinuous() { return setMode(INA226_SHUNTBUSCONTINUOUS_MODE); };   //  default.


  //  Alert
  //  - separate functions per flag?
  //  - what is a reasonable limit?
  //  - which units to define a limit per mask ?
  //    same as voltage registers ?
  //  - how to test
  bool     setAlertRegister(uint16_t mask);
  uint16_t getAlertFlag();
  bool     setAlertLimit(uint16_t limit);
  uint16_t getAlertLimit();


  //  Meta information
  uint16_t getManufacturerID();   //  should return 0x5449
  uint16_t getDieID();            //  should return 0x2260


  //  DEBUG
  uint16_t getRegister(uint8_t reg) { return _readRegister(reg); };


private:

  uint16_t         _readRegister(uint8_t reg);
  uint16_t         _writeRegister(uint8_t reg, uint16_t value);
  float            _current_LSB;
  float            _shunt;
  float            _maxCurrent;

  ina226_mode_enum _mode;
  ina226_avg_enum  _avg;
  ina226_ct_enum   _bvct;
  ina226_ct_enum   _svct;

  uint8_t          _address;
  TwoWire *        _wire;
};


//  -- END OF FILE --

