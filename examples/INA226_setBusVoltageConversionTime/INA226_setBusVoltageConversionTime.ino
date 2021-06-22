//
//    FILE: INA226_BusVoltageConversionTime.ino
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.1
// PURPOSE: demo
//    DATE: 2021-05-18
//     URL: https://github.com/RobTillaart/INA226


//  run this sketch in the IDE plotter
//  change the setBusVoltageConversionTime(7)  (line 33)   0..7
//  change the bus voltage
//  0 reads fast  ...  7 staircasing, slower reads)


#include "INA226.h"
#include "Wire.h"

INA226 INA(0x40);


void setup()
{
  Serial.begin(115200);
  Serial.println(__FILE__);

  Wire.begin();
  if (!INA.begin() )
  {
    Serial.println("could not connect. Fix and Reboot");
  }
  INA.setMaxCurrentShunt(1, 0.002);
  INA.setBusVoltageConversionTime(7);    //   <<<<<<<
}


void loop()
{
  Serial.print(INA.getBusVoltageConversionTime());
  Serial.print("\t");
  Serial.print(INA.getBusVoltage(), 4);
  Serial.println();
  delay(100);
}


// -- END OF FILE --
