#pragma once
//    FILE: INA266.h
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.0
//    DATE: 2021-05-18
// PURPOSE: Arduino library for INA266 power sensor
//     URL: https://github.com/RobTillaart/INA226


#include "Arduino.h"
#include "Wire.h"


#define INA226_LIB_VERSION         (F("0.1.0"))


class INA226
{
public:
  explicit INA226(const int8_t address, TwoWire *wire = &Wire);

#if defined (ESP8266) || defined(ESP32)
  bool     begin(const uint8_t sda, const uint8_t scl);
#endif

  bool     begin();
  bool     isConnected();

  // core Functions
  float    getShuntVoltage();
  float    getBusVoltage();
  float    getPower();
  float    getCurrent();

  // calibration & configuration
  // read datasheet for details
  void     reset();
  void     setAverage(uint8_t avg = 0);
  void     setBusVoltageConversionTime(uint8_t bvct = 4);
  void     setShuntVoltageConversionTime(uint8_t svct = 4);
  // TODO ?? GETTERS?


  // operating mode
  void     setMode(uint8_t mode = 7);
  uint8_t  getMode();
  void     shutDown()                 { setMode(0); };
  void     setModeShuntTrigger()      { setMode(1); };
  void     setModeBusTrigger()        { setMode(2); };
  void     setModeShuntBusTrigger()   { setMode(3); };
  void     setModeShuntContinuous()   { setMode(5); };
  void     setModeBusContinuous()     { setMode(6); };
  void     setModeShuntBusContinuous(){ setMode(7); };  // default.


  // alert ....
  // TODO.



  // meta information
  uint16_t getManufacturerID();   // should return 0x5449
  uint16_t getDieID();            // should return 0x2260


private:

  uint16_t _readRegister(uint8_t reg);
  uint16_t _writeRegister(uint8_t reg, uint16_t value);


  uint8_t   _address;
  TwoWire * _wire;

};

// -- END OF FILE --
