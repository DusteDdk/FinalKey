/*
  The Final Key is an encrypted hardware password manager, 
  this is the sourcecode for the firmware. 

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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