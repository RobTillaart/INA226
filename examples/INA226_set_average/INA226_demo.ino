//
//    FILE: INA226_set_average.ino
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.0
// PURPOSE: demo
//    DATE: 2021-05-18
//     URL: https://github.com/RobTillaart/INA226


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

  Serial.print("SHUNT:\t");
  Serial.println(INA.getShuntVoltage(), 2);
  Serial.print("  BUS:\t");
  Serial.println(INA.getBusVoltage(), 2);
  Serial.print("POWER:\t");
  Serial.println(INA.getPower(), 2);
  Serial.print(" CURR:\t");
  Serial.println(INA.getCurrent(), 2);

  Serial.println();
  Serial.print("MAN:\t");
  Serial.println(INA.getManufacturerID(), HEX);
  Serial.print("DIE:\t");
  Serial.println(INA.getDieID(), HEX);

  Serial.println("now set average");
}


void loop()
{
  for (int avg = 0; avg < 8; avg++)
  {
    INA.setAverage(avg);
    Serial.print("  AVG:\t");
    Serial.println(avg);
    Serial.print("SHUNT:\t");
    Serial.println(INA.getShuntVoltage(), 2);
    Serial.print("  BUS:\t");
    Serial.println(INA.getBusVoltage(), 2);
    Serial.print("POWER:\t");
    Serial.println(INA.getPower(), 2);
    Serial.print(" CURR:\t");
    Serial.println(INA.getCurrent(), 2);
    Serial.println();
    delay(1000);
  }
}



// -- END OF FILE --
