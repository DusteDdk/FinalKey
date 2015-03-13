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

#ifndef EncryptedStorage_H
#define EncryptedStorage_H
#include <I2ceep.h>
#include <AES.h>

//Each entry is 16 bytes for iv, followed by encrypted struct of  bytes 
//Struct must be %16 == 0
typedef struct {
  char title[32]; // Title needs to be two blocks for seperate decryption
  uint8_t seperator;		//Character to type between user and pass (ignored when macro)
  uint8_t passwordOffset;	//Where the password starts in the string of data (0 = macro)
  char data[190];
} entry_t;

class EncryptedStorage
{
public:
  void begin(); //Initialize entropy and power on eeprom
  bool readHeader(char* deviceName); //If eeprom is ready to use, puts the saved deviceName into deviceName, and return true. False = format.
  bool unlock( byte* k ); //Returns true if it was possible to decrypt using that key, enables further decryption
  void lock();
  bool getTitle( uint8_t entryNum, char* title); //Only decrypts the name (for speed)
  bool getEntry( uint8_t entryNum, entry_t* entry ); //Reads and decrypts an entry, return true if entry is valid, otherswise entry is empty.
  void putEntry( uint8_t entryNum, entry_t* entry ); //Encrypts and Writes an entry.
  void changePass( byte* newKey, byte* oldPass );
  void delEntry ( uint8_t entryNum ); //Deletes an entry
  void format( byte* pass, char* name ); //A 32 byte key and a 32 byte name
  void setBanner(char* banner);
  void exportData();
  void importData();
  uint16_t getNextEmpty();
  void setKeyboardLayout(uint8_t lang);
  uint8_t getKeyboardLayout();
private:
  void putPass( byte* pass );
  void putIv( byte* dst );
  AES aes;
  uint8_t crc8(const uint8_t *addr, uint8_t len);  
};

extern EncryptedStorage ES;

#endif
