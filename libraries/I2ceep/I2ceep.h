#ifndef __I2ceep_H__
#define __I2ceep_H__
#include <Arduino.h>

#define I2E_Write(addr, data, len) I2E.dataOp(addr, data, len, 1)
#define I2E_Read(addr, data, len) I2E.dataOp(addr, data, len, 0)

class I2ceep
{
public:
  void power(uint8_t state);
  
  //Requires page-aligned eeaddress, any len valid, returns addresss after last byte read/written.
  uint16_t dataOp(uint16_t eeaddress, byte* data, uint8_t len, uint8_t write);

private:
  //Requires page-aligned eeaddress + len max 32 bytes.
  //void writeQuarterPage(uint16_t eeaddress, byte* data, uint8_t len);
  //void readQuarterPage(uint16_t  eeaddress, byte* data, uint8_t len);

};
extern I2ceep I2E;

#endif