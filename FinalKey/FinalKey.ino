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

#include <TermTool.h>
#include <Entropy.h>
#include <AES.h>
#include <Wire.h>
#include <EncryptedStorage.h>
#include <I2ceep.h>
#include <avr/eeprom.h>
#include <sha256.h>

//Edit this file to select included layouts.
#include <KeyMaps.h>


#define ledPin 10
#define btnPin 9
#define btnPwr 7

#define BTN_NO 0
#define BTN_YES 1

#define CMD_FIRE_PASS 1
#define CMD_FIRE_BOTH 2
#define CMD_SHOW_BOTH 3
#define CMD_ACCOUNT_LIST_NEXT 4
#define CMD_ACCOUNT_LIST_PREV 5
#define CMD_LOCK_DEV 6
#define CMD_HELP 7
#define CMD_NEW_ACCOUNT 8
#define CMD_FORMAT_DEVICE 10
#define CMD_DELETE_ENTRY 11
#define CMD_OVERRIDE_ENTRY 12
#define CMD_SET_BANNER 14
#define CMD_IMPORT_DATA 15
#define CMD_EXPORT_DATA 16
#define CMD_EXTENDED 17
#define CMD_ROBOT 18
#define CMD_SEARCH_TRIG 19
#define CMD_FIRE_USER 20

//Timeout time (in milliseconds) for things that require a bit extra attention from the user.
#define BTN_TIMEOUT_IMPORTANT 5000
//Timeout for other stuff
#define BTN_TIMEOUT_FIRE 30000
//Wait forever (and ever)
#define BTN_TIMEOUT_NO_TIMEOUT -1
//How many ms should the button be held before it means "abort", I like this one.
#define BTN_HOLD_TIME_FOR_ABORT 700

//fireEntry will set these
uint8_t lastEntryCmd[]={0,0,0,0}, lastEntryIdx=0;
uint16_t lastEntryNum[]={0,0,0,0};



const char clsStr[] = { 27, '[', '2','J',27,'[','H',0 };

//Actually saves a bit of memory to have this function and the char array, instead of printing it each time, clears the screen, don't abuse.
void cls()
{
  Serial.print(clsStr);
}

void clearSerialInput()
{
  while(Serial.available())
  {
    Serial.read();
    delay(20); //Wait a bit, host may have more buffered.
  }  
}

//These are the characters that can be used in passwords, they are arranged in this way for easy implementation of the optional "use specials" feature of the password generator.
const char passChars[] = {
'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E',
'F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T',
'U','V','W','X','Y','Z','a','b','c','d','e','f','g','h','i',
'j','k','l','m','n','o','p','q','r','s','t','u','v','w','x', //last normal as number 62 (idx 61)
'y','z','!','"','#','$','%','&','@','?','(',')','[',']','-', // <75 (idx 75)
'.',',','+','{','}','_','/','<','>','=','|','\'','\\', 
';',':',' ','*'// <- 92 (idx 91)
};

//Reads string from serial, zero terminates the string
//Returns on enter, numChars is characters BEFORE 0 terminator ( so for a char[32], you write 31 m'kay?)
//Returns false if user has entered more than numChars
bool getStr( char* dst, const uint8_t numChars, bool echo )
{
  uint8_t keycheck;
  char inchar;
  uint8_t index=0;

  memset( dst, 0, numChars+1 );
  while( 1 )
  {
    if( Serial.available() )
    {
      inchar = Serial.read();

      if( inchar == 8 ) //If backspace is pressed
      {
        if( index > 0 )
        {
          --index;
          dst[index]=0;
        }
      } else if( inchar == 13 ) //If Enter is pressed
      {
        return(1);
      } else if( index == numChars ) //If this key makes the string longer than allowed
      {
        return(0);
      } else {
        for(keycheck=0; keycheck < 92; keycheck++ )
        {
          if(passChars[keycheck] == inchar)
          {
            break;
          }
        }
        if(keycheck == 92 )
        {
          ptxtln("\r\n[InvalChar]");
          return(0);
        } else {

          dst[index++] = inchar;
          
        }
      }

      if(echo)
      {
        Serial.write('\r');
        Serial.write(27);
        Serial.write("[K");
        Serial.write(dst);
      }

    }
    if(!Serial)
    {
        return(0);
    }
  }
}

void printKbLayoutList()
{
#ifdef KBMAP_A
  ptxtln(KBMAP_A_NAME);
#endif
#ifdef KBMAP_B
  ptxtln(KBMAP_B_NAME);
#endif
#ifdef KBMAP_C
  ptxtln(KBMAP_C_NAME);
#endif
#ifdef KBMAP_D
  ptxtln(KBMAP_D_NAME);
#endif
}

void getKbLayout()
{
  uint8_t k;
  while(1)
  {
    ptxtln("Layout:");
    printKbLayoutList();
  ptxt("% ");
    
    k = getOneChar()-'0';
      switch(k)
      {
        #ifdef KBMAP_A
        case 1:
          k=KBMAP_A;
        break;
        #endif
        #ifdef KBMAP_B
        case 2:
          k=KBMAP_B;
        break;
        #endif
        #ifdef KBMAP_C
        case 3:
          k=KBMAP_C;
        break;
        #endif
        #ifdef KBMAP_D
        case 4:
          k=KBMAP_D;
        break;
        #endif
        default:
          k=INVALID_KEYBOARD_LAYOUT;
        break;
      }
      if( k == INVALID_KEYBOARD_LAYOUT )
      {
        printError();
      } else {
        kbmaps.setKbMap(k);
        if( testChars() )
        {
          ES.setKeyboardLayout(k);
          printOk();
          return;
        }
      }
  }//While 1 ends here
}

//User-friendly format process.
//This function must not exit before a successfull format.
void format()
{
  //Two input buffers, for easy password comparison.
  char bufa[33];
  char bufb[33];
  ptxtln("Master (1-32)");
  bool fail=1;

  while(fail)
  {
    ptxt("Psw:");
    if( getStr(bufa, 32,0 ) )
    {
      ptxt("\r\nAgain:");
      if( getStr(bufb, 32,0 ) )
      {
        if(memcmp(bufa,bufb,32)==0)
        {
          fail=0;
        }
      }
    }
    if(fail)
      printError();
  }
  fail=1;
  while(fail)
  {
    ptxtln("\r\nName (0-31):");
    if( getStr(bufb, 31,1 ) )
    {
      fail=0;
    }
  }
  Serial.println();
  getKbLayout();
  ES.format( (byte*)bufa, bufb ); 
}


//Waits for button to be pressed, returns either BTN_NO (for timeout and aborted) or BTN_YES
uint8_t btnWait( int timeOut )
{
  int btnAbortTimeLeft=BTN_HOLD_TIME_FOR_ABORT;
  
 
  uint8_t state=1,onTime=0;
  uint8_t ret = BTN_NO;
  if(timeOut > 0)
  {
    Serial.println();
  }
  
  while( timeOut != 0 )
  {
    if( (timeOut % 1000)==0 && timeOut > 0)
    {
      ptxt("\r");txt((int)(timeOut/1000));ptxt(" # ");
    }
    
    //Blink the led
    delay(1);
    digitalWrite(ledPin, state);
    onTime++;
    if(onTime==100)
    {
      onTime=0;
      state=!state;
    }
    
    //Button pin uses internal pullup, switch drags pin to gnd.
    if( !digitalRead(btnPin) )
    {
      btnAbortTimeLeft--;
      if(btnAbortTimeLeft==0)
      {
        break;
      }
    } else if(btnAbortTimeLeft!=BTN_HOLD_TIME_FOR_ABORT)
    {
        ret=BTN_YES;
        break;
    } else if(timeOut > 0 )
    {
      timeOut--;
    }
  }


  

  clearSerialInput();
  
  if(!Serial)
  {
    ret=BTN_NO;
  } 

  digitalWrite(ledPin, HIGH);
  Serial.println("\r       \r");
  return(ret);
}

uint8_t login(bool header)
{
  char key[33];
  char devName[32];
  memset(key,0,33);
  memset(devName,0,32);
  uint8_t ret=0;

  if(header)
    cls();
   //Try reading the header.
  if( !ES.readHeader(devName) )
  {
    ptxtln("[Format]");
    format();
  } else {
    if(header)
    {
      Serial.write('{');
      Serial.print(devName);
      ptxt("}\r\nPsw:");
    }
    getStr( key,32, 0 );
   
    if( ES.unlock( (byte*)key ) )
    {
       ret=1;
       if(header)
        {
          kbmaps.setKbMap(ES.getKeyboardLayout());
        }        
     } else {
        printError();
        delay(2000);
     }
  }
  
  return(ret);
}


void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(btnPwr, OUTPUT);

  Entropy.initialize();

  Wire.begin();
  Serial.begin(9600);

  analogWrite(ledPin, 250);
  randomSeed( Entropy.random() );
  digitalWrite(ledPin,1);

  digitalWrite(btnPwr, LOW); //Sink 
  pinMode(btnPin, INPUT); // set pin to input

  //Enable internal pullups for button
  digitalWrite(btnPin, HIGH);
  
  
}

void beat()
{
  uint8_t pwm=0;
  uint8_t t=0;
  for(pwm=255; pwm != 64; pwm--)
  {
    analogWrite(ledPin, pwm);
    delay(1);
  }
  for(pwm=64; pwm != 255; pwm++)
  {
    analogWrite(ledPin, pwm);
    delay(2);
  }
  digitalWrite(ledPin, HIGH);

  
}

bool isHex( char c )
{
return( (c >('0'-1) && c < ('9'+1) ) || (c > ('a'-1) && c < ('f'+1) )  );
}


uint8_t strToHex( char* str )
{
  uint8_t ret;
  char c;
  if (c=(str[0]-'0'),(c>=0 && c <=9)) ret=c;
  else if (c=(str[0]-'a'),(c>=0 && c <=5)) ret=(c+10);
  ret = ret << 4;
  if (c=(str[1]-'0'),(c>=0 && c <=9)) ret|=c;
  else if (c=(str[1]-'a'),(c>=0 && c <=5)) ret|=(c+10);

  return(ret);
}

bool testChars()
{
  int i;
  char* spec="Specials:";
  ptxt("\r\nFocus blank input, press btn or long-press to skip test.\r\n#");
  if(btnWait(BTN_TIMEOUT_NO_TIMEOUT))
  {
    Serial.print(spec);
    Keyboard.print(spec);
    for(i=62; i < 92; i++)
    {
      Serial.print(passChars[i]);
      delay(10);
      Keyboard.press(passChars[i]);
      Keyboard.release(passChars[i]);
    }
    ptxt("\r\nLayout correct if identical to above.\r\nCorrect [y/N] ?");
    if( getOneChar() == 'y' )
    {
      return(1);
    } else {
      return(0);
    }
  } else {
    ptxt("\r\n[skip]");
  }
}

void putRandomChars( char* dst, uint8_t len, uint8_t useSpecial, char* specials )
{
  char pool[256];
  long maxRv = (useSpecial)?91:61; //Not numbers but indexes

  ptxt("\r\n[RANDGEN]\r");
  memcpy( pool, passChars, maxRv );
  
  if( specials )
  {
    memcpy( (pool+maxRv), specials, strlen(specials));
    maxRv += strlen(specials);
  }

  for( uint8_t idx = 0; idx < len; idx++)
  {
    dst[idx] = pool[(uint8_t)Entropy.random(maxRv)];
  }
  printOk();
  
}

void fireEntry(uint8_t what, int16_t entryNum)
{
  char eName[33];
  entry_t entry;
  if( entryNum == -1 )
  {
    printAbort();
    return;
  }

  if( ES.getTitle(entryNum, eName ) )
  {
    uint8_t i;
    for(i=0; i<4;i++)
    {
      if( lastEntryCmd[i] == what && lastEntryNum[i] == entryNum )
      {
        i=10;
      }
    }
    if(i==4)
    {
      lastEntryCmd[lastEntryIdx]=what;
      lastEntryNum[lastEntryIdx]=entryNum;
      lastEntryIdx++;
      if(lastEntryIdx>3)
      {
        lastEntryIdx=0;
      }
    }
    
    ptxtln("\r\n");    
    switch(what)
    {
      case CMD_FIRE_BOTH:
        ptxt("[U][S][P] ");
        break;
      case CMD_FIRE_USER:
        ptxt("[U] ");
        break;
      case CMD_FIRE_PASS:
        ptxt("[P] ");
        break;
      case CMD_SHOW_BOTH:
        ptxt("[SHOW] ");
        break;
    }
    txt(eName);
    ptxtln(" ?");
    
    if(btnWait(BTN_TIMEOUT_FIRE))
    {
      ES.getEntry(entryNum, &entry);
            
      //Empty serial input buffer
      clearSerialInput();
      
      if( what == CMD_FIRE_BOTH || what == CMD_FIRE_USER )
      {
        Keyboard.print( entry.data );
        ptxt(" [U]");
      }
      
      if( what == CMD_FIRE_BOTH )
      {
        Keyboard.write( entry.seperator );
        delay(150);
        ptxt(" [S]");
      }
      
      if( what == CMD_FIRE_BOTH || what == CMD_FIRE_PASS )
      {
        Keyboard.print( (entry.data)+entry.passwordOffset );
        ptxt(" [P]");
      }
      
      if( what == CMD_FIRE_BOTH )
      {
        Keyboard.write(10);
        ptxt(" [E]");
      }
      

      
      if( what == CMD_SHOW_BOTH )
      {
        ptxt("\r\nAccount: ");          
        if(entryNum < 16)
        {
          Serial.write('0');
        }          
        Serial.print(entryNum,HEX);
        Serial.print(" - ");
        Serial.print(eName);
        
        ptxt("\r\n  Usr: ");
        Serial.print( entry.data );
        ptxt("\r\n  Psw: ");
        Serial.print( (entry.data)+entry.passwordOffset );
        ptxt("\r\n");
      }
 
      //Detect if finalkey typed into its own terminal.
      if( Serial.available() )
      {
        clearSerialInput();     
        ptxt("\r\n[Focus]");
        printError();
      } else {
        Serial.println();
        printOk();
      }
    } else {
      printAbort();
    }
  } else {
    ptxtln("\r\n[empty]");
  }
}

uint8_t getOneChar()
{
  while(Serial)
  {
    if(Serial.available())
    {
      //Handle backspace (delete last character)
      if( Serial.peek() == 8 )
      {
        Serial.write(8);
        Serial.write(' ');
        Serial.write(8);
      } else {
        Serial.write(Serial.peek());
      }
      return(Serial.read());
    }
  }
  return(255);
}

//Returns -1 on failure
int collectNum()
{
  char inp[2];
  char c;
  uint8_t i=0;
  while(1)
  {
    c=getOneChar();
    if( c==8 )
    {
      if(i==0)
      {
        return(-1);
      }
      i=0;
    } else if( !isHex(c) )
    {
      return(-1);
    } else {
      inp[i++]=c;
      if(i==2)
      {
        break;
      }
    }
  }
  Serial.println();  
  return(strToHex(inp));
}


void newAccount(int entryNum)
{
  char inp[4];
  char spec[21];
  entry_t entry;
  uint8_t r;
  bool save=0;
  uint8_t maxLen;

  if(entryNum == -1 )
  {
    entryNum = ES.getNextEmpty();
    if( entryNum == 256 )
    {
      ptxtln("\r\n[full]");
      return;
    }    
    
    if( !btnWait( BTN_TIMEOUT_IMPORTANT ) )
    {
      goto ABORT_NEW_ACCOUNT;
    }
  }
    
  
  ptxtln("Title (0-31):");
  if( getStr( entry.title, 31, 1 ) )
  {
    ptxtln("\r\nUsr:");
    if( getStr(entry.data, 189, 1 ) )
    {
      entry.passwordOffset = strlen(entry.data)+1;
      maxLen = 189-(entry.passwordOffset); //190 - 1 for password and - the offset

      ptxt("\r\nType? \r\n 1 = manual\r\n 2 = auto\r\n%");

          r=getOneChar();
          if(r=='1')
          {
            ptxt("\r\nPsw, (1-");
            Serial.print( maxLen );
            ptxtln("):");
            if(getStr( ((entry.data)+entry.passwordOffset), maxLen, 0 ) )
            {
              save=1;
            }

          } else if(r=='2')
          {
            ptxt("\r\nLen, (1-");
            Serial.print( maxLen );
            ptxtln("):");
            if( getStr( inp, 3, 1 ) )
            {
              if( (atoi( inp ) < (maxLen+1) && atoi(inp) > 0) && strlen(inp)!=0 )
              {
                ptxt("\r\nSpecials?\r\n 1 = All\r\n 2 = Choose\r\n 3 = None\r\n%");
                
                r=getOneChar()-'0';
                if( r > 0 && r < 4 )
                {
                  if( r== 2) //Choose
                  {
                    ptxtln("\r\nSpecials, (1-20):");
                    if( getStr( spec, 20, 1 ) )
                    {
                      putRandomChars( ((entry.data)+entry.passwordOffset),atoi(inp), 0, spec );
                      save=1;
                    }
                  } else {
                    putRandomChars( ((entry.data)+entry.passwordOffset),atoi(inp), (r==1), 0 );
                    save=1;
                  } //None or All
                  
                } //Correct input for auto-recipe
              } //Valid length for auto
            } //Got string for pass len
          } //Auto pass
          
          //Get seperator

          if(save)
          {
            ptxt("\r\nSeperator?\r\n 1 = [TAB]\r\n 2 = [ENT]\r\n%");
  
            r=getOneChar()-'0';
            if( r==1 )
            {
              entry.seperator=9;
            } else if(r==2)
            {
              entry.seperator=10;
            } else {
              save=0;
            }
          }

          if( save )
          {
            ptxt("\r\n[save ");      
            Serial.print(entryNum, HEX);
            txtln("]");

            ES.putEntry( (uint8_t)entryNum, &entry );

            printOk();
            return;
          }


    } //Got username
  } //Got title
  
  ABORT_NEW_ACCOUNT:
  printAbort();
}


void delEntry(bool override)
{
  char ename[32];
  int en = collectNum();
  uint8_t entryType=0;
  entry_t entry;
  
  if(en != -1)
  {
    if( ES.getTitle((uint8_t)en,ename) )
    {
      if(override)
      {
        ptxt("Replace ");
      } else {
        ptxt("Del ");
      }
      txt(ename);
      ptxt(" [y/N] ?");

      if( getOneChar() == 'y' )
      {

        if( btnWait( BTN_TIMEOUT_IMPORTANT ) )
        {
          ES.delEntry((uint8_t)en);
          if(override)
          {
            newAccount(en);
          } else {
            printOk();
          }
          return;
        }
      }
    }
  }

  printAbort();
}




void setBanner()
{
  char banner[32];

  if( btnWait( BTN_TIMEOUT_IMPORTANT ) )
  {
    ptxtln("\r\nBanner (0-31):");
    if(getStr( banner,31,1) )
    {
      ES.setBanner(banner);
      printOk();
      return;
    }
  }
  
  printAbort();
}

uint8_t page=0;

void entryList(int8_t dir)
{
  cls();
  char eName[32];
  bool col=0;
  if( dir ==-1 && page == 0 )
  {
    page = 5;
  } else if( dir == 1 && page == 5 )
  {
    page = 0;
  } else if(  (dir < 0 && page > 0 ) || (dir > 0 && page < 5 ) )
  {
    page+=dir;
  }
  uint16_t offset = page*44;

  
  ptxt("Accounts [");
  txt(page);
  txt("/5]\r\n");
  
  uint8_t len = 0;
  
  uint8_t pageEnd=offset+44;

  while( offset < pageEnd )
  {
    if( ES.getTitle((uint8_t)offset, eName) )
    {
    
      if( col )
      {
        uint8_t spaces = 43-len;
        for(uint8_t i=0; i < spaces; i++)
        {
          Serial.write(' ');
        }
      } else {
        Serial.write(' ');
      }

      if( offset<0x10 )
      {
        Serial.write('0');
      }
      Serial.print(offset, HEX);
      Serial.print(" - ");
      len = 6;

        len+=strlen(eName);
        Serial.print(eName);
      
        
      if(col)
      {
       Serial.print("\r\n"); 
       col=0;
      } else {
        col=1;
      }
    }
    offset++;
  }
  
 if( col )
 {
   Serial.println();
 }

}

void machineList()
{
  char eName[32];

  uint16_t i;
      Serial.println();      
  for(i=0; i < 256; i++)
  {
    if( ES.getTitle((uint8_t)i, eName) )
    {
      if( i < 0x10 )
      {
        Serial.write('0');
      }
      Serial.print(i,HEX);
      Serial.print(eName);
      Serial.println();
    }
  }
  Serial.println("[KBL]");
  printKbLayoutList();
  
 Serial.write('>');
}

//Convert a string to uppercase for easy comparison by the search feature.
void strToUpper(char* str)
{

  while( *str )
  {
    if( (*str) > 96 && (*str) < 123 )
    {
      (*str)-=32;
    }
    str++;
  }
    
}
void search(uint8_t cmd)
{
  char str[32];
  char eName[32];
  char ueName[32];
  uint8_t slen, elen, hits=0, fire;

  ptxtln("\rSearch:      ");

  if( getStr( str, 31, 1 ) )
  {
    if(strlen(str) < 1)
    {
      printAbort();
      return;
    }
    strToUpper(str);

    Serial.println();
    slen=strlen(str);
    for( int16_t idx = 0; idx < 256; idx++ )
    {
      if( ES.getTitle( (uint8_t)idx, eName ) )
      {
        elen=strlen(eName);
        memcpy(ueName, eName, 32);
        strToUpper(ueName);
        for(uint8_t o=0; o< (elen-slen)+1; o++)
        {
          if( memcmp(ueName+o, str, slen) == 0 )
          {
            fire=idx;
            hits++;
            ptxt("Found: ");
            if(idx < 16)
            {
              Serial.write('0');
            }
            Serial.print(idx,HEX);
            ptxt(" - ");
            txt(eName);
            Serial.println();
            break;
          }
        }
      }
    }

    switch(hits)
    {
      case 1:
        fireEntry(cmd, fire);
      break;
      case 0:
        ptxt("[not found]");
      break;
      default:
        ptxt("[found ");txt(hits);ptxt("]");
      break;
    }
    return;
  }
  ptxt("\r\n[too long");
}

bool changePass()
{
  char newPassA[33];
  char newPassB[33];
  char oldPass[33];
  
  cls();
  ptxt("DANGER: Re-encrypts data.\r\nData will be lost if interrupted.\r\nContinue [y/N] ?");
  if( getOneChar() == 'y' )
  {

    if(btnWait(BTN_TIMEOUT_IMPORTANT) )
    {
      ptxt("\r\nCur psw:");
      getStr( oldPass, 32, 0 );
      if( ES.unlock( (byte*)oldPass ) )
      {
        ptxt("New psw:");
        if( getStr(newPassA, 32,0 ) )
        {
          ptxt("\r\nAgain:");
          if( getStr(newPassB, 32,0 ) )
          {
            if(memcmp(newPassA,newPassB,32)==0)
            {
              ptxtln("\r\nDo not disconnect.");
              ES.changePass( (byte*)newPassA, (byte*)oldPass );
              return(1);
            }
          }
        }      
      }
    }
  }
  
  return(0);
}


void repeatEntry(bool show)
{
  uint8_t i,cur;
  char eName[33];
  entry_t entry;
  cur=lastEntryIdx;
  if(cur==0)
  {
    cur=4;
  }

  cur--;
  if( lastEntryCmd[cur] != 0 )
  {
    ptxtln("\r\n\r\n[repeat]");
    for(i=0; i<4;i++)
    {
      if( lastEntryCmd[i] != 0 )
      {
        if(cur==i)
        {
          ptxt(" *");
        } else {
          ptxt("  ");
        }
        Serial.print(i+1);
        ptxt(". (");
        if(lastEntryNum[i] < 0x10)
        {
          Serial.write('0');
        }
        
        Serial.print(lastEntryNum[i], HEX);
  
        ES.getTitle(lastEntryNum[i], eName);
        ptxt(") - ");
        Serial.write(eName);
        ptxt( " [");
        switch( lastEntryCmd[i] )
        {
          case CMD_FIRE_USER:
            ptxt("U");
          break;
          case CMD_FIRE_PASS:
            ptxt("P");
          break;
          case CMD_FIRE_BOTH:
            ptxt("U][S][P");
          break;
          case CMD_SHOW_BOTH:
            ptxt("SHOW");
          break;
        }

        ptxt( "]");
      }
    }
 
    if(show)
    {
      ptxt("[%/ENT] %");
      i=getOneChar();
      if(i>'0' && i<'5')
      {
        i -='1';
      } else if(i==13)
      {
        i=cur;
      } else {
        printAbort();
        return;
      }
    } else {
      i=cur;
    }
    fireEntry(lastEntryCmd[i], lastEntryNum[i] );    
  } else {
    ptxtln("\r\n[none yet]");
  }
}



void loop()
{
  char cmd[4];
  uint8_t p=0;
  uint8_t cmdType=0;


  //Wait for serial connection
  while( !Serial );
  cls();
  beat();


  //If the firmware flash function is enabled, warn the user about it.
  if( eeprom_read_byte((uint8_t*)0) != 'F' )
  {
    ptxtln("[Flashable]");
  }
  //Greet user
  ptxt("Final Key+\r\n#");
  
  if( !btnWait(BTN_TIMEOUT_NO_TIMEOUT) )
  {
    return;
  }


  
  //Turn on power to EEPROM
  I2E.power(HIGH);

   
  //Login procedure
  if( login(1) )
  {
  
    ptxt("Space = quickhelp\r\n>");
    
    //Interpret commands and call appropiate functions.
    while(Serial)
    {      
      while(Serial.available())
      {
       cmd[p]=Serial.read();
       if( cmd[p] > 31 && cmd[p] < 127 )
       {
         Serial.write(cmd[p]);
       }
       
       //Handle backspace, we abort whatever was going on.
       if(cmd[p]==8)
       {
         Serial.write('\r');
         Serial.write(27);
         Serial.write("[K");
        p=0;
       } else {
         p++;
       }

       //Check for 1 character commands
       if(p==1)
       {

         if(cmd[0]==' ')
         {
           cls();
           ptxtln("The Final Key\r\n-------------\r\n u  Usr\r\n p  Psw\r\n %  Usr+Psw\r\n s  Show\r\n r  Repeat list\r\n R  Repeat last\r\n j  list <\r\n k  list \r\n l  list >\r\n q  Lock\r\n h  Help\r\n ENTER  Search");
           p=0;
         } else if(cmd[0]=='u')
         {
           Serial.write('%');
           fireEntry(CMD_FIRE_USER, collectNum() );
           p=0;
         } else if(cmd[0]=='p')
         {
           Serial.write('%');
           fireEntry(CMD_FIRE_PASS, collectNum() );
           p=0;           
         } else if( cmd[0] == 's' )
         {
           Serial.write('%');
           fireEntry(CMD_SHOW_BOTH, collectNum() );

           p=0;
         } else if( cmd[0] == 'j' )
         {
           entryList(-1);
           p=0;
         } else if( cmd[0] == 'l' )
         {
           entryList(1);
           p=0;
         } else if( cmd[0] == 'k' )
         {
           entryList(0);
           p=0;
         } else if( cmd[0] == 'q' )
         {
           cls();
           ES.lock();
           ptxt("[lock]");
           delay(2000);
           return;
         } else if( cmd[0] == 'h' )
         {
           cls();
           ptxtln("Help\r\n----\r\n Space  quickhelp\r\n ENTu  Search and trig usr\r\n ENTp  Search and trig psw\r\n ENTs  Search and show\r\n ENTENT  Search and trig both\r\n xa  New account\r\n xf  Format\r\n xp  Change psw\r\n xd  Delete\r\n xo  Override\r\n xb  Set banner\r\n xk  Set keyboard layout\r\n ------------\r\n # = Button on The Final Key\r\n % = Number  : = Text Input\r\n ENT = ENTER > = Command\r\n ------------");
           p=0;
         } else if( isHex(cmd[0]) )
         {
           cmdType=CMD_FIRE_BOTH;
          } else if( cmd[0] == 'x' )
         {
           cmdType=CMD_EXTENDED;
         } else if( cmd[0] == 13 )
         {
           ptxt("\r[ENT/u/p/s]:");
           cmdType=CMD_SEARCH_TRIG;
         }else if( cmd[0] == 'X' )
         {
          ptxtln("[auto]");
          cmdType=CMD_ROBOT; 
         } else if( cmd[0] == 'r' )
         {
           repeatEntry(1);
           p=0;
         } else if( cmd[0] == 'R' )
         {
           repeatEntry(0);
           p=0;
         } else {
           ptxt("\r\n[unknown]");
           p=0;
         }
       } else
       //Check for 2 character commands and entry numbers
       if(p==2)
       {
         if( cmdType==CMD_FIRE_BOTH )
         {
           if(isHex(cmd[1]))
           {
             fireEntry(CMD_FIRE_BOTH, strToHex(cmd));
             p=0;
           }
         } else if( cmdType==CMD_EXTENDED )
         {
           switch(cmd[1])
           {
             case 'a':
               newAccount(-1);
             break;
             case 'f':
               cls();
               ptxt("WARNING: DESTROYS ALL DATA!\r\nSure [y/N] ?");
               if(getOneChar() == 'y' && btnWait(BTN_TIMEOUT_IMPORTANT) )
               {
                 ES.lock();
                 ptxt("\r\nCur psw:");
                 if( login(0) )
                 {
                   ptxt("\r\n");
                   format();
                 } else {
                   return;
                 }
               } else {
                printAbort();
               }
             break;
             case 'p':
               if(!changePass())
               {
                 ES.lock();
                 return;
               }
             break;
             case 'd':
               txt("%");
               delEntry(0);
             break;
             case 'o':
               txt('%');
               delEntry(1);
             break;
             case 'b':
               txt(':');
               setBanner();
             break;
             case 'k':
               getKbLayout();
             break;
             case 'e':

                 ptxt("\r\n\r\nFlash:\r\n  Press and hold: unlock\r\n  Short press: lock.\r\n\r\n#");

                 if( btnWait(BTN_TIMEOUT_NO_TIMEOUT ) )
                 {
                   eeprom_write_byte(0,'F');
                   eeprom_write_byte((uint8_t*)1,'K');
                   ptxtln("[locked]");
                 } else {
                   eeprom_write_byte(0,0);
                   eeprom_write_byte((uint8_t*)1,0);
                   ptxtln("[unlocked]");
                 }

             break;
               default:
               ptxtln("[unknown]");
             break;
           }
           
           p=0;
         } else if( cmdType == CMD_SEARCH_TRIG )
         {
         //  cmd[0] = getOneChar();
           if( cmd[1] == 'u' )
           {
             search(CMD_FIRE_USER);
           } else if( cmd[1] == 'p' )
           {
             search(CMD_FIRE_PASS);
           } else if( cmd[1] == 13 ) //Enter key
           {
             search(CMD_FIRE_BOTH);
           } else if( cmd[1] == 's' )
           {
             search(CMD_SHOW_BOTH);
           } else {
             ptxt("\r\n[abort]");
           }
         } else if( cmdType == CMD_ROBOT )
         {
           switch( cmd[1] )
           {
             case 'i':
               ptxtln("[RDY]");
               if( btnWait(BTN_TIMEOUT_IMPORTANT) )
               {
                 printOk();
                 ES.importData();
                 return;
               } else {
                 printError();
               }
               p=0;
             break;
             case 'e':
             ptxtln("[RDY]");
             if( btnWait(BTN_TIMEOUT_IMPORTANT) )
             {
               printOk();
               ES.exportData();
             } else {
               printError();
             }
             break;
             case 'l': //List for machines
             machineList();
             break;
             case 'r': //Random source
             while( !Serial.available() )
             {
               Serial.println(Entropy.random(0xff),HEX);
             }
             break;
           }
         }
         
         p=0;
       } //Second character 

        if(p==0)
        {
          txt(">");
        }
       
      } //Incoming char
      
      
    } //Serial connected

  } //Device unlocked
  
  
  //Turn off power to EEPROM
  I2E.power(LOW);

}

