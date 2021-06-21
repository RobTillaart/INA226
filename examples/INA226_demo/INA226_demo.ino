//
//    FILE: INA226_demo.ino
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.1
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

  Serial.println();
  Serial.print("MAN:\t");
  Serial.println(INA.getManufacturerID(), HEX);
  Serial.print("DIE:\t");
  Serial.println(INA.getDieID(), HEX);
  delay(100);
  INA.setMaxCurrentShunt(1, 0.002);
  Serial.print("LSB:\t");
  Serial.println(INA.getCurrentLSB(), 10);
  Serial.println("\n\n");

  Serial.println("BUS\tSHUNT\tCURRENT\tPOWER");
}


void loop()
{
  //  for (int r = 0; r < 6; r++)
  //  {
  //    Serial.print(INA.getRegister(r), HEX);
  //    Serial.print('\t');
  //  }
  //  Serial.println();

  Serial.print(INA.getBusVoltage(), 3);
  Serial.print("\t");
  Serial.print(INA.getShuntVoltage_mV(), 3);
  Serial.print("\t");
  Serial.print(INA.getCurrent_mA(), 3);
  Serial.print("\t");
  Serial.print(INA.getPower_mW(), 3);
  Serial.println();
  delay(1000);
}


// -- END OF FILE --
