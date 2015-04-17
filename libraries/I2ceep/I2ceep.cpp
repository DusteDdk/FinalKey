/*
  The Final Key is an encrypted hardware password manager,
  This lib handles EEPROM r/w in 32 byte page-aligned chuncks.

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

#include <Wire.h>
#include "I2ceep.h"

#define EEPROMA0  A0
#define EEPROMA1  A1
#define EEPROMA2  A2
#define EEPROMVSS A3

#define EEPROMVCC  5
#define EEPROMWP   4
#define EEPROMSCL  3
#define EEPROMSDA  2

#define EEPROMPULLUP 15

#define EEP_I2C_ADDR 80 //This is 160 >> 1, we have 1010 and then 000 as cs.


uint16_t I2ceep::dataOp(uint16_t eeaddress, byte* data, uint8_t len, uint8_t write)
{
  
///  Serial.print("\r\n[dataOp from address " ); Serial.print(eeaddress); Serial.print(" bytes: "); Serial.print(len); Serial.print("\r\n");
  while(len)
  {
    uint8_t lenForPage;
    lenForPage = (len > 31)?32:len;
    
    uint8_t currentPageOffset = eeaddress%128;
    uint8_t nextPageOffset = (eeaddress+lenForPage)%128;
    
    if( nextPageOffset < currentPageOffset )
    {
      lenForPage -= nextPageOffset;
    }
    
    //Start communication with eeprom, send address
    Wire.beginTransmission(EEP_I2C_ADDR); //Last one tells to WRITE  
    Wire.write((uint16_t)(eeaddress >> 8)); // MSB
    Wire.write((uint16_t)(eeaddress & 0xFF)); // LSB    
    
    
    if(write)
    {
      //Write the length for current page
      for(uint8_t i = 0; i < lenForPage; i++)
      {
	Wire.write(data[i]);
///     Serial.print("[");Serial.print(data[i]);Serial.print("]");
      }
      Wire.endTransmission();
      data+=lenForPage;
///      Serial.print("ioop <write> ");
    } else {
      //Stop and request data back from address.
      Wire.endTransmission();
      Wire.requestFrom((uint8_t)(EEP_I2C_ADDR),(uint8_t)lenForPage);
      while( Wire.available() )
      {
	*(data++)=Wire.read();
      }
///      Serial.print("ioop <read> ");
    }

   //I observed that there were no errors after successive writing if having 4ms pause,
   //with the eeproms I use, there were at 3 ms though, so I give it an extra ms to be sure.
   delay(4);
    
///    Serial.print(lenForPage);Serial.print(" bytes from address ");Serial.print(eeaddress);
///    Serial.print("\r\n");

    eeaddress+=lenForPage;
    
    len -= lenForPage;
  }
///  Serial.print("[dataOp Finished Next Address is " ); Serial.print(eeaddress); Serial.print("\r\n");
  
  return(eeaddress);
}

void pinOutState(uint8_t pin, uint8_t state)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, state);  
}

//Turns off and on the eeprom chip, since it is connected to gpio's we can do that, so why not ?
void I2ceep::power(uint8_t state)
{
  //Set gnd to addresses and vss and writeprotect
  pinOutState(EEPROMA0, LOW);  
  pinOutState(EEPROMA1, LOW);  
  pinOutState(EEPROMA2, LOW);  
  pinOutState(EEPROMVSS, LOW);  
  pinOutState(EEPROMWP, LOW);

  //Set +5 to VCC and pullup
  pinOutState(EEPROMVCC, state);
  pinOutState(EEPROMPULLUP, state);  

  //Give the eeprom some time to start up.
  delay(10);
  Wire.setClock(400000);
}

I2ceep I2E = I2ceep();
