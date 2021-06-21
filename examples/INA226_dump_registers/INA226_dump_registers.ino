//
//    FILE: INA226_dump_registers.ino
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.0
// PURPOSE: demo
//    DATE: 2021-06-21
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

  Serial.println("\n\tREG\tVALUE\tVALUE_X");
  for (int r = 0; r < 6; r++)
  {
    Serial.print('\t');
    Serial.print(r);
    Serial.print('\t');
    Serial.print(INA.getRegister(r), DEC);
    Serial.print('\t');
    Serial.println(INA.getRegister(r), HEX);
  }
  Serial.println();
}


void loop()
{
}


// -- END OF FILE --
