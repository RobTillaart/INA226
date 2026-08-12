
## INA226_USI

### Credits

Derived class developed by Peter.Simoons, 2026-08-08

### Description

The INA226_TINY implements the INA226 library (based upon 0.6.6) for the Atmel ATtiny85 et al. 
The INA226 library was adapted to implement a small voltage and current monitor for a power supply, and works well.

For now the working name is INA226_USI.

### ChangeLog

#### 2026-08-08 [0.6.6 based]
- initial implementation. ATtiny85 uses TinyWireM and USI_TWI_Master for I2C. These replaces the TwoWire I2C API. 


### Testing

The library is only tested with ATtiny85 hardware.
A simple test sketch compiled well for the ATtiny44/45/64/65/84, although
not tested on hardware.

Feedback as always is welcome.


