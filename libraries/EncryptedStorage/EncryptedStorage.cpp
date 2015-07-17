/*
  The Final Key is an encrypted hardware password manager, 
  This lib handles encryption/decryption/eeprom layout. 

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

#include "EncryptedStorage.h"
#include <I2ceep.h>
#include <Entropy.h>
#include <util/atomic.h>
#include <Wire.h>
#include <TermTool.h>
#include <sha256.h>

//#define it to turn on verbose loggin (you need to disable some keyboard layouts because there is not enough flash)
#undef DEBUG_PRINT

//512 iterations, about 11 seconds
//256 iterations, about 5.5 seconds
//128 iterations, about 3 seconds
#define KEY_HASH_ITERATIONS 200

#define HEADER_EEPROM_IDENTIFIER_LEN 12
const static char eepromIdentifierTxt[HEADER_EEPROM_IDENTIFIER_LEN] PROGMEM  =  "[FinalKey3]";



#define HEADER_SIZE 256
//We reserve 1024 bytes for a rainy day
#define EEPROM_ENTRY_START_ADDR 1280
#define ENTRY_SIZE 224
#define EEPROM_ENTRY_DISTANCE 240 //EntrySize + 16 for iv
#define ENTRY_FULL_CBC_BLOCKS 14 //Blocksize / 16 for encryption
#define ENTRY_NAME_CBC_BLOCKS 2 //Blocksize of decryption of title
#define NUM_ENTRIES 256


#define FALSE 0
#define TRUE 1

//Header:
//0-11  	- Identifier (12 bytes)
//12-43 	- Devicename (32 bytes)
//44-59		- Mac Iv (16 bytes)
//60-91		- Encrypted MAC (32 bytes)
//92-123	- Cleartext MAC (32 bytes)
//124-187       - Salt (64 bytes)
//188-188	- Keyboard Layout (1 byte)

#define EEPROM_IDENTIFIER_LOCATION 0
#define EEPROM_DEVICENAME_LOCATION 12

#define EEPROM_IV_LOCATION 44
#define EEPROM_MAC_CIPHER_LOCATION 60
#define EEPROM_MAC_CLEARTEXT_LOCATION 92
#define EEPROM_SALT_LOCATION 124

#define EEPROM_KEYBOARD_LAYOUT_LOCATION	188

//Read the IV which comes after 12 + 32 bytes and is 16 bytes long. (Identifier + Unit name)
#define getMacIv(iv) I2E_Read( EEPROM_IV_LOCATION, iv, 16 )
#define getMacCipher(ciph) I2E_Read( EEPROM_MAC_CIPHER_LOCATION, ciph, 32 )
#define getMacClear(mac) I2E_Read( EEPROM_MAC_CLEARTEXT_LOCATION, mac, 32 )
#define getSalt(salt) I2E_Read( EEPROM_SALT_LOCATION, salt, 64 );

#define entryOffset( entryNum ) ((EEPROM_ENTRY_START_ADDR)+(EEPROM_ENTRY_DISTANCE*entryNum))

#if defined(DEBUG_PRINT)
void debPr(char* str, byte* dat, int len)
{
    Serial.println(str);
    for(int i=0; i < len; i++)
    {
        if( dat[i] < 0xf )
        {
            ptxt("0");
        }
      Serial.print((uint8_t)dat[i], HEX);
    }
    Serial.println(); 
}

#else
  #define debPr //
#endif


bool EncryptedStorage::readHeader(char* deviceName)
{
  byte buf[HEADER_EEPROM_IDENTIFIER_LEN];
  uint16_t offset = I2E_Read(0, buf, HEADER_EEPROM_IDENTIFIER_LEN);
  
  for(uint8_t i = 0; i < HEADER_EEPROM_IDENTIFIER_LEN; i++)
  {
    if( buf[i] != pgm_read_byte(& eepromIdentifierTxt[i])  )
    {
      return(FALSE);
    }
  }
  I2E_Read(offset, (byte*)deviceName, 32);
  
  return(TRUE);
}

void EncryptedStorage::setBanner(char* banner)
{
  I2E_Write( HEADER_EEPROM_IDENTIFIER_LEN,(byte*)banner, 32 );
}


bool EncryptedStorage::unlock( byte* pass )
{
  byte key[32];
  byte clearMac[32];
  byte mac[32];
  byte iv[16];

  debPr("pass ", pass, 32);
  //Get the MAC IV
  getMacIv(iv);
  debPr("iv ", iv, 16);

  //Get the MAC cleartext
  getMacClear(clearMac);
  debPr("clearMac ", clearMac, 32);
  
  //Get the encrypted mac
  getMacCipher(mac);
  debPr("encmac ", mac, 32);

  //Make the key
  genKey(pass, key);

  //Set the key
  aes.set_key(key, 256);

  //Decrypt the encrypted mac
  aes.cbc_decrypt (mac, mac, 2, iv);
  debPr("decmac ", mac, 32);

  //Check if they match
  if( memcmp(mac, clearMac, 32) != 0 )
  {
    return(FALSE);
  }

  return(TRUE);
}


void EncryptedStorage::lock()
{
  aes.clean();
}


static uint16_t getIVandStartAddressForEntry( uint8_t entryNum, byte* iv )
{
  uint16_t offset = entryOffset(entryNum);
  offset = I2E_Read( offset, iv, 16 );
  return(offset);
}

static bool ivIsEmpty( byte* iv )
{
  uint8_t r = 0;
  for(uint8_t i=0; i<16;i++)
  {
    r |= iv[i];
  }
  return( (r==0) );
}


bool EncryptedStorage::getTitle( uint8_t entryNum, char* title)
{
  byte iv[16];
  byte cipher[32];
  uint16_t offset = getIVandStartAddressForEntry( entryNum, iv);
    
  if( ivIsEmpty( iv ) )
  {
    return(FALSE);
  }
  
  //Read first 32 bytes of entry.
  I2E_Read( offset, cipher, 32 );
  
  //Decrypt title
  aes.cbc_decrypt( cipher, (byte*)title, ENTRY_NAME_CBC_BLOCKS, iv );

  return(TRUE);
}


bool EncryptedStorage::getEntry( uint8_t entryNum, entry_t* entry )
{
  byte iv[16];
  uint16_t offset = getIVandStartAddressForEntry( entryNum, iv);
  if( ivIsEmpty( iv ) )
  {
    return(FALSE);
  }  

  //Read entry
  I2E_Read( offset, (byte*)entry, ENTRY_SIZE );
  
  //Decrypt entry
  aes.cbc_decrypt( (byte*)entry,(byte*)entry, ENTRY_FULL_CBC_BLOCKS, iv );
  return(TRUE);
}


void EncryptedStorage::putEntry( uint8_t entryNum, entry_t* entry )
{
  uint16_t offset = entryOffset(entryNum);
  //Create IV
  byte iv[16];
  putIv(iv);
  
  //Write IV
  offset=I2E_Write( offset , iv, 16 );
  
  //Encrypt entry
  aes.cbc_encrypt((byte*)entry,(byte*)entry, ENTRY_FULL_CBC_BLOCKS, iv);
  
  //Write entry
  I2E_Write( offset,(byte*)entry,  ENTRY_SIZE );
}


void EncryptedStorage::delEntry(uint8_t entryNum)
{
  uint16_t offset = entryOffset(entryNum);
  entry_t dat;
  entry_t dat2;

  memset(&dat,0,16); //Zero out first 16 bytes of entry so we can write an all zero iv.  
  //Write an all zero iv to indicate it's empty
 // Serial.print("\r\nEntry num: "); Serial.print(entryNum);Serial.print(" iv offset before ");Serial.print(offset);
  offset = I2E_Write( offset, (byte*)&dat, 16 );
  
  //Fill entry with random numbers
  for(uint8_t i=0; i < ENTRY_SIZE; i++)
  {
    ((byte*)(&dat))[i] = random(255);
  }
  
  //Write random data
  I2E_Write( offset, (byte*)&dat, ENTRY_SIZE );
  //Read it back
  I2E_Read( offset, (byte*)&dat2, ENTRY_SIZE );
  
  //Compare, to see that we read what we wrote
  if( memcmp( &dat, &dat2, ENTRY_SIZE ) !=  0 )
  {
     strcpy( dat.title, "<BAD>" );
     dat.passwordOffset=0;
     putEntry( entryNum, &dat );
  }

}

void EncryptedStorage::changePass( byte* newPass, byte* oldPass )
{
  char unused[32];
  entry_t entry;
  
  byte oldKey[32];
  byte newKey[32];

  genKey(oldPass, oldKey);
  
  putPass(newPass);

  genKey(newPass, newKey);

  // For each non-empty entry
  printBusy();
  for(uint16_t i = 0; i < 256; i++ )
  {
  // Serial.write('\r');txt(i);Serial.write('/');txt(255);
    if( getTitle( (uint8_t)i, unused ) )
    {
      //Set old key
      aes.set_key( oldKey, 256 );
      
      //Read entry
      getEntry( (uint8_t)i, &entry );

      //Set new key
      aes.set_key( newKey, 256 );

      //Write entry
      putEntry( (uint8_t)i, &entry );

    }
  }
  
  printOk();
}

void EncryptedStorage::format( byte* pass, char* name )
{
  byte identifier[HEADER_EEPROM_IDENTIFIER_LEN];

  //Delete Entries
  for(uint16_t i=0; i < NUM_ENTRIES; i++ )
  {
    delEntry((uint8_t)i);
  }
  //Serial.print(F("Entries Written\r\n"));

  putPass(pass);

  //Copy Identifier to memory
  for(uint8_t i=0; i < HEADER_EEPROM_IDENTIFIER_LEN; i++)
    identifier[i]=pgm_read_byte (& eepromIdentifierTxt[i]);
  
  //Write the cleartext stuff. Identifier and Name.
  I2E_Write( EEPROM_IDENTIFIER_LOCATION, identifier, HEADER_EEPROM_IDENTIFIER_LEN );
 // Serial.print(F("Identifier Written\r\n"));
  I2E_Write( EEPROM_DEVICENAME_LOCATION,(byte*)name, 32 );
 // Serial.print(F("Name Written\r\n"));
}


//Find used and all 0  IV's so we can avoid them (0 avoided because we use it for detecting empty entry)
static bool ivIsInvalid( byte* dst )
{
  bool invalid = FALSE;
  byte iv[16];
  
  //check against all zero, all zero is unused entry
  if(ivIsEmpty(dst))
  {
    invalid=TRUE;
  }
  
  //The first iv is the one for the MAC in the header.
  getMacIv(iv);
  if(memcmp(iv, dst, 16) == 0)
  {    
    invalid=TRUE;
  } else {
    //Loop thorugh the entries
    for( uint16_t e = 0 ; e < NUM_ENTRIES; e++ )
    {
      getIVandStartAddressForEntry(e, iv);
      if( memcmp( iv, dst, 16 ) == 0)
      {
	invalid=TRUE;
	break;
      }
    }
  }

  return(invalid);
}


//Generate a key
//Sure, too few iterations, and the hash should be expensive,
//Reality check: This is on a 16 mhz 8 bit micro, nothing this chip can do will be expensive.
//Pick a 32 byte password and be happy.
void EncryptedStorage::genKey(byte* pass, byte* keyDest )
{
  byte* hash;
  byte code[64];
  
  ptxt("\r[KEYGEN]");
  //Read the salt  
  getSalt(code);    
  debPr("salt ", code, 64);

  //XOR password into the salt
  for(uint8_t i = 16 ; i < 48; i++ )
  {
    code[i] ^= pass[i-16];
  }
  debPr("unhashed ", code, 64);
  
  //Chew on it a bit
  for(int i=0; i < KEY_HASH_ITERATIONS; i++ )
  {
    hash = Sha256.hash( code, 64 );

   //Xor it with the previous hash
   for(int p=0; p<32; p++)
   {
       if(p%2==0)
       {
        code[p] ^= (hash[p]|0xF0 ); 
        code[p+32] ^= (hash[p]|0x0F ); 
       } else {
        code[p+32] ^= (hash[p]|0xF0 ); 
        code[p] ^= (hash[p]|0x0F ); 
       }
       debPr("step ", code, 64);
   }
    
  }
  //Hash it once more, and copy it to the destination
  hash = Sha256.hash( code, 64 );
  memcpy(keyDest, hash, 32);
  ptxt("\r        \r");
}

void EncryptedStorage::putPass( byte* pass )
{

    
  byte code[64];
  byte iv[16];
  byte mac[32];

  debPr("pass ", pass,32);
  //Generate salt for the code
  for(uint8_t i = 0; i < 64; i++)
  {
    code[i]=Entropy.random(0xff);
  }
  
  //Write the salt now
  I2E_Write( EEPROM_SALT_LOCATION, code, 64 );
  
  //Create the key, reuse first 32 bytes of code
  genKey(pass, code);
  

  //Generate IV for MAC
  putIv( iv );
  debPr("iv ", iv, 16);

  //Write the IV before it's changed by the encryption algorithm.
  I2E_Write( EEPROM_IV_LOCATION, iv, 16 );

  //Generate the MAC
  putIv( mac );
  putIv( mac+16 );
  debPr("clearmac ", mac, 32);


  //Write the MAC
  I2E_Write( EEPROM_MAC_CLEARTEXT_LOCATION, mac, 32 );
  
  //Encrypt the mac.
  aes.set_key(code, 256);   //First 256 (32 byte) bit of the code is now the hashed key
  aes.cbc_encrypt(mac, code+32, 2, iv); //Last 32 bytes of the code is now the cipher
  debPr("encmac ", code+32, 32);

  //Write mac cipher
  I2E_Write(EEPROM_MAC_CIPHER_LOCATION, code+32, 32);
  
  //Write background noise
  ///I2E_Write(EEPROM_PASS_BACKGROUND_LOCATION, bck, 32);  
  
 
  
}

void EncryptedStorage::putIv( byte* dst )
{
  do {
    for(uint8_t i = 0; i < 16; i++)
    {
      dst[i]=Entropy.random(0xff);
    }

  } while( ivIsInvalid(dst) );
}

uint8_t EncryptedStorage::crc8(const uint8_t *addr, uint8_t len)
{
        uint8_t crc = 0;

        while (len--) {
                uint8_t inbyte = *addr++;
                for (uint8_t i = 8; i; i--) {
                        uint8_t mix = (crc ^ inbyte) & 0x01;
                        crc >>= 1;
                        if (mix) crc ^= 0x8C;
                        inbyte >>= 1;
                }
        }
        return crc;
}


//R = Ready, W = Wait, C =CRC error, O = OK, F = Failure, E=End
void EncryptedStorage::importData()
{
  char buf[32];
  uint16_t i=0;
  uint8_t crca, crcb;
  Serial.setTimeout(30000);
  while( i < 64000 )
  {
    txt("R");
    if( Serial.readBytes(buf,32) == 32 )
    {
      crcb = Serial.read();
      txt("W");
      crca = crc8((uint8_t*)buf,32);
      if( crca != crcb )
      {
	I2E_Write( 0, (byte*)"C", 1 );
	txt("C");
	while(1) {};
      } else {
	txt("O");
      }
      I2E_Write(i, (byte*)buf, 32);
      i+=32;
    } else {
      I2E_Write( 0, (byte*)"F", 1 );
      txt("F");
      break;
    }
  }
  txt("E");
}

void EncryptedStorage::exportData()
{
  byte buf[32];
  uint8_t crc;
  ptxt("[FK]"); 
  for(uint16_t i=0; i < 64000; i+=32)
  {
    I2E_Read( i, buf, 32 );
    for( int ii=0; ii < 32; ii++)
    {
      Serial.write((char)buf[ii]);
    }
    crc = crc8( (uint8_t*)buf, 32 );
    Serial.write( (char)crc );
  }
  ptxt("[KF]"); 
}

uint16_t EncryptedStorage::getNextEmpty()
{
  uint8_t iv[16];
  uint16_t idx;
  uint8_t taken;

  for(idx=0; idx < 256; idx++)
  {
    //Read iv
    getIVandStartAddressForEntry(idx, iv);
    
    taken=0;
    for(uint8_t i = 0; i < 16; i++)
    {
      taken |= iv[i];
    }
    
    if( !taken )
    {
      break;
    }

  }
  
  return( idx );
}

void EncryptedStorage::setKeyboardLayout(uint8_t lang)
{
  byte buf = (byte)lang;
  I2E_Write(EEPROM_KEYBOARD_LAYOUT_LOCATION, &buf, 1);  
}

uint8_t EncryptedStorage::getKeyboardLayout()
{
  byte buf;
  I2E_Read( EEPROM_KEYBOARD_LAYOUT_LOCATION, &buf, 1 ); 
  return( (uint8_t)buf);
}

EncryptedStorage ES = EncryptedStorage();
