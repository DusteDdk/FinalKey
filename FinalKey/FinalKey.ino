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
#define CMD_NEW_MACRO 9
#define CMD_FORMAT_DEVICE 10
#define CMD_DELETE_ENTRY 11
#define CMD_OVERRIDE_ENTRY 12
#define CMD_SET_MACRO 13
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

void setRng(boolean doSet)
{
  uint8_t attempts=0;
  if( doSet )
  {
    //Wait for Entropy to be available.
    analogWrite(ledPin, 128);
    while( !Entropy.available() )
    {
        delay(100);
        attempts++;
        if(attempts==30) //If there's no random number after 3 seconds, re-initialize the rng.
        {
          analogWrite(ledPin, 192);
          Entropy.Initialize();
          attempts=0;
        }
    }
    randomSeed( Entropy.random() );
  }
  digitalWrite(ledPin,1);
}

bool getStr( char* dst, const uint8_t numChars, bool echo )
{
  uint8_t keycheck;
  char inchar;
  uint8_t index=0;
  char scramble=0;
  boolean shouldSetRng=false;
  memset( dst, 0, numChars+1 );
  while( 1 )
  {
    
    if( !digitalRead(btnPin) )
    {
      shouldSetRng=true;
      while( !digitalRead(btnPin) ) { };
      scramble++;
      randomSeed(scramble);
      digitalWrite(ledPin, 0);
      delay(50);
    }

    if( Serial.available() )
    {
      inchar = Serial.read();

      if( inchar == 8 ) //If backspace is pressed
      {
        //If backspace is pressed when scamble is on, delete the whole string.
        if(scramble)
        {
          scramble=0;
          digitalWrite(ledPin, 1);
          index=0;
          memset( dst, 0, numChars+1 );
          Serial.print("<<\r\n");
        } else if(index > 0 )
        {
          dst[--index]=0;
        }
      } else if( inchar == 13 ) //If Enter is pressed
      {
        setRng(shouldSetRng);
        return(1);
      } else if( inchar == 27 ) //If Escape is pressed
      {
        index=0;
        memset( dst, 0, numChars+1 );
      } else if( index == numChars ) //If this key makes the string longer than allowed
      {
        goto GETSTR_RETERR;
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
          ptxt("\r\n[Unsupported:");Serial.print(inchar);ptxtln("]");
          goto GETSTR_RETERR;
        } else {
          
          if(scramble)
          {
            inchar = (inchar^random(254)+1)&0xFF;
          }
          
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
        goto GETSTR_RETERR;
    }
  }
  
  GETSTR_RETERR:
    setRng(shouldSetRng);
    return(0);
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
    ptxtln("Select keyboard layout:");
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
        ptxtln("\r\n[Invalid choice]");
      } else {
        kbmaps.setKbMap(k);
        if( testChars() )
        {
          ES.setKeyboardLayout(k);
          ptxtln("\r\n[saved]");
          return;
        }
      }
    Serial.println();
  }//While 1 ends here
}

//User-friendly format process.
//This function must not exit before a successfull format.
void format()
{
  //Two imput buffers, for easy password comparison.
  char bufa[33];
  char bufb[33];
  ptxtln("Choose password (1-32)");
  ptxtln("It is important that you remember this password.\r\nThe Final Key can not be unlocked without it.");
  bool fail=1;

  while(fail)
  {
    ptxt("Psw:");
    if( getStr(bufa, 32,0 ) )
    {
      ptxt("\r\nRepeat:");
      if( getStr(bufb, 32,0 ) )
      {
        if(memcmp(bufa,bufb,32)==0)
        {
          fail=0;
        }
      }
    }
    if(fail)
      ptxtln("\r\n[ERROR]");
  }
  fail=1;
  while(fail)
  {
    ptxtln("\r\nName, (0-31):");
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
  //Blink the led
  digitalWrite(ledPin, state);

  

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
    ptxtln("Need format before use");
    format();
  } else {
    if(header)
    {
      Serial.write('{');
      Serial.print(devName);
      ptxt("}\r\nPass:");
    }
    getStr( key,32, 0 );
   
    if( ES.unlock( (byte*)key ) )
    {
       ret=1;
       if(header)
        {
          ptxtln("\r\n[Granted]");
          kbmaps.setKbMap(ES.getKeyboardLayout());
        }        
     } else {
       if(header)
       {
         ptxtln("\r\n[Denied]");
       }
     }
  }
  
  return(ret);
}


void setup() {
  
  Entropy.Initialize();
  Wire.begin();
  setRng(true);  
  Serial.begin(9600);
  
  pinMode(ledPin, OUTPUT);
  pinMode(btnPwr, OUTPUT);
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
  ptxt("\r\nTo verify the selected layout works, focus a blank text-field\r\nthen press the key or long-press to skip the test.\r\n#");
  if(btnWait(BTN_TIMEOUT_NO_TIMEOUT))
  {
    ptxt("Supported specials:");
    Keyboard.print("Supported specials:");
    for(i=62; i < 92; i++)
    {
      Serial.print(passChars[i]);
      delay(10);
      Keyboard.press(passChars[i]);
      Keyboard.release(passChars[i]);
    }
    ptxt("\r\nLayout correct if identical to above line.\r\nCorrect [y/n] ?");
    if( getOneChar() == 'y' )
    {
      return(1);
    } else {
      return(0);
    }
  } else {
    ptxt("\r\n[skip test]");
  }
}

void putRandomChars( char* dst, uint8_t len, uint8_t useSpecial, char* specials )
{
  char pool[256];
  long maxRv = (useSpecial)?91:61; //Not numbers but indexes
    
  memcpy( pool, passChars, maxRv );
  
  if( specials )
  {
    memcpy( (pool+maxRv), specials, strlen(specials));
    maxRv += strlen(specials);
  }
  
  ptxtln("\r\n[generate]"); 
  //Wait for entropy pool to fill
  while(Entropy.available()!=8);
  
  for( uint8_t idx = 0; idx < len; idx++)
  {
    Serial.print("\r[");
    txt(idx);
    Serial.write('/');
    txt(len);
    Serial.write(']');
    dst[idx] = pool[(uint8_t)Entropy.random(maxRv)];
  }
  ptxtln("\r[done]   ");
  
}

void fireEntry(uint8_t what, int16_t entryNum, bool noWait)
{
  char eName[33];
  entry_t entry;
  if( entryNum == -1 )
  {
    ptxt("[abort]\r\n>");
    return;
  }
  REPEAT_FIRE:
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
      default:
        ptxt("[ERROR]");
        break;
    }
    txt(eName);
    ptxt(" ?\r\n");
    
    if(noWait || btnWait(BTN_TIMEOUT_FIRE))
    {
      ES.getEntry(entryNum, &entry);
      
      if( entry.passwordOffset == 0 && entry.seperator == 0 )
      {
        Keyboard.print( entry.data );
        ptxt(" [M]");
      } else {
      
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
          
          ptxt("\r\n  Username: ");
          Serial.print( entry.data );
          ptxt("\r\n  Password: ");
          Serial.print( (entry.data)+entry.passwordOffset );
          ptxt("\r\n");
        }
      }
      
        //Empty serial input buffer (in case user triggered it to write into the finalkey)
        if( Serial.available() )
        {
          clearSerialInput();     
          ptxt("\r\n[ERROR] Wrong window, try again.");
          //Wohoo! A GOTO!! YES! FINALLY! (I could make it recursive, and risk stack overflow, or make a nasty while around the function, but I decided this was less noisy)
          goto REPEAT_FIRE;
        } else {
          ptxt(" [done]\r\n>");
        }
    } else {
      ptxt("[abort]\r\n>");
    }
  } else {
    ptxt("[empty]\r\n>");
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
    
  
  ptxtln("Account Title, (0-31):");
  if( getStr( entry.title, 31, 1 ) )
  {
    ptxtln("\r\nUser:");
    if( getStr(entry.data, 189, 1 ) )
    {
      entry.passwordOffset = strlen(entry.data)+1;
      maxLen = 189-(entry.passwordOffset); //190 - 1 for password and - the offset

      ptxt("\r\nPsw type? \r\n 1 = manual\r\n 2 = auto\r\n%");

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
            ptxt("\r\nSeperator?\r\n 1 = [TAB]\r\n 2 = [ENT]\r\n 3 = Choose\r\n%");
  
            r=getOneChar()-'0';
            if( r==1 )
            {
              entry.seperator=9;
            } else if(r==2)
            {
              entry.seperator=10;
            } else if(r==3)
            {
              ptxt("\r\n>");
              entry.seperator = getOneChar();
            } else {
              save=0;
            }
          }

          if( save )
          {
            ptxt("\r\n[save entry ");
            Serial.print(entryNum, HEX);
            txtln("]");

            ES.putEntry( (uint8_t)entryNum, &entry );

            ptxtln("[done]");
            return;
          }


    } //Got username
  } //Got title
  
  ABORT_NEW_ACCOUNT:
  
  ptxtln("\r\n[abort]");
}


void newMacro(int16_t entryNum)
{
  entry_t entry;
  char data[190];
  bool save=0;
  memset(data,0,190);

  if(entryNum == -1 )
  {
    entryNum = ES.getNextEmpty();
    
    if( entryNum== 256 )
    {
      ptxtln("\r\n[full]");
      return;
    }
    
    if( !btnWait( BTN_TIMEOUT_IMPORTANT ) )
    {
      goto ABORT_NEW_MACRO;
    }

  }

  ptxtln("\r\nMacro Title (0-31):");
  if( getStr( entry.title, 31, 1 ) )
  {
    ptxtln("\r\nEmpty line with . to save:");
    uint8_t offset=0;
    while( 1 )
    {
      if(getStr(data+offset, (189 - offset),1))
      {
        if( strlen( (data+offset) ) == 1 && *(data+offset)=='.' )
        {
          data[offset-1]=0; //Remove the \n and . at the end ;)
          save=1;
          break;
        } else {
          offset = strlen(data);
          data[offset]='\n';
          offset++;
          if( (189 - offset) < 2 )
          {
            ptxtln("\r\nTxt too long!");
            goto ABORT_NEW_MACRO;
          }
          ptxt("\r\n[Free:");
          txt( (189 - offset)-2 );
          ptxtln("]");
        }
      } else {
        goto ABORT_NEW_MACRO;
      }
    }

    
  }
  
  
  if(save)
  {
    ptxt("\r\n[save entry ");
    Serial.print(entryNum, HEX);
    txtln("]");
    entry.passwordOffset=0;
    entry.seperator=0;
    memcpy( entry.data, data, 190 );
    ES.putEntry( entryNum, &entry );
    ptxtln("[done]");
    return; 
  }
  
  ABORT_NEW_MACRO:
  ptxtln("\r\n[abort]");
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
        ptxt("Override ");
      } else {
        ptxt("Del ");
      }
      txt(ename);
      ptxtln(" [y/n] ?");

      if( getOneChar() == 'y' )
      {

        if( btnWait( BTN_TIMEOUT_IMPORTANT ) )
        {
          ES.getEntry((uint8_t)en, &entry);
          ES.delEntry((uint8_t)en);
          if(override)
          {
            if(entry.seperator==0 && entry.passwordOffset==0)
            {
              newMacro(en);
            } else {
              newAccount(en);
            }
          } else {
            txt("\r\n[deleted]");
          }
          return;
        }
      }
    } else {
      ptxtln("[empty]");
      return;
    }
  }

  ptxtln("[abort]");
}

int16_t macroNum = -1;

void setMacro()
{
  entry_t entry;
  int en = collectNum();
  
  if( en != -1 )
  {
    if( ES.getTitle( en,(entry.title) ) )
    {
      ES.getEntry((uint8_t)en, &entry);
      if(entry.seperator!=0 || entry.passwordOffset!=0)
      {
        ptxtln("Not a macro, but you're the boss.");
      }
      macroNum=(uint8_t)en;
      ptxtln("[set]");
      return;
    } else {
      ptxtln("[empty]");
    }
  }
  ptxtln("[abort]");
  
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
      ptxtln("\r\n[done]");
      return;
    }
  }
  
  ptxt("\r\n[abort]");
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
 Serial.write('>');
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
      ptxt("[empty keyword]\r\n>");
      return;
    }
    strToUpper(str);
    ptxtln("\r\n[search]");
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
        fireEntry(cmd, fire, 0 );
      break;
      case 0:
        ptxt("[not found]\r\n>");
      break;
      default:
        ptxt("[Found ");txt(hits);ptxt("]\r\n>");
        ptxt("[keyword ambiguous]\r\n>");
      break;
    }
    return;
  }
  ptxt("\r\n[keyword too long]\r\n>");
}

void changePass()
{
  char newPassA[33];
  char newPassB[33];
  char oldPass[33];
  
  cls();
  ptxt("WARNING: This re-encrypts data, it takes around 3 seconds per account.\r\nIf power is lost while re-encrypting, data will be lost.\r\nDon't forget your new psw.\r\nAre you sure [y/n] ?");
  if( getOneChar() == 'y' )
  {

    if(btnWait(BTN_TIMEOUT_IMPORTANT) )
    {
      ptxtln("\r\nCurrent psw:");
      getStr( oldPass, 32, 0 );
      if( ES.unlock( (byte*)oldPass ) )
      {
        ptxtln("\r\nNew psw:");
        if( getStr(newPassA, 32,0 ) )
        {
          ptxt("\r\nRepeat:");
          if( getStr(newPassB, 32,0 ) )
          {
            if(memcmp(newPassA,newPassB,32)==0)
            {
              ptxtln("\r\nDo not disconnect.");
              ES.changePass( (byte*)newPassA, (byte*)oldPass );
              return;
            }
          }
        }      
      }
    }
  }
  
  ptxtln("\r\n[abort]");
  
}



void repeatEntry(uint8_t show)
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

        ptxt( "]\r\n");
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
        ptxt("[abort]\r\n>");
        return;
      }
    } else {
      i=cur;
    }
    fireEntry(lastEntryCmd[i], lastEntryNum[i], 0 );    
  } else {
    ptxt("\r\n[not set]\r\n>");
  }
}



void loop()
{
  char cmd[4];
  uint8_t p=0;
  uint8_t cmdType=0;
  uint8_t btnCoolDown=200;
  

  macroNum = -1;



  beat();


  //Wait for serial connection
  while( !Serial );
  cls();
  beat();


  //If the firmware flash function is enabled, warn the user about it.
  if( eeprom_read_byte((uint8_t*)0) != 'F' )
  {
    ptxtln("[NOTE: flash unlocked]");
  }
  //Greet user
  ptxt("The Final Key\r\n#");

  if( !btnWait(BTN_TIMEOUT_NO_TIMEOUT) )
  {
    return;
  }

  
  //Turn on power to EEPROM
  I2E.power(HIGH);

   
  //Login procedure
  if( login(1) )
  {
    ptxt("Space for quickhelp\r\n>");
    
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
        Serial.write("[K>");
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
           ptxt("The Final Key\r\n-------------\r\n u  Usr\r\n p  Psw\r\n %  Usr+Psw\r\n s  Show\r\n r  Repeat recent\r\n R  Repeat last\r\n j  list <\r\n k  list \r\n l  list >\r\n q  Lock\r\n h  Help\r\n ENTER  Search\r\n>");
           p=0;
         } else if(cmd[0]=='u')
         {
           Serial.write('%');
           fireEntry(CMD_FIRE_USER, collectNum(), 0 );
           p=0;
         } else if(cmd[0]=='p')
         {
           Serial.write('%');
           fireEntry(CMD_FIRE_PASS, collectNum(), 0 );
           p=0;           
         } else if( cmd[0] == 's' )
         {
           Serial.write('%');
           fireEntry(CMD_SHOW_BOTH, collectNum(), 0 );

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
           delay(500);
           return;
         } else if( cmd[0] == 'h' )
         {
           cls();
           ptxt("Help\r\n----\r\n Space  quickhelp\r\n ENTu  Search and trig usr\r\n ENTp  Search and trig psw\r\n ENTs  Search and show\r\n ENTENT  Search and trig both\r\n xa  New account\r\n xm  New macro\r\n xf  Format\r\n xp  Change psw\r\n xd  Delete\r\n xo  Override\r\n xu  Choose # macro\r\n xb  Set banner\r\n xk  Set keyboard layout\r\n ------------\r\n # = Button on The Final Key\r\n % = Number  : = Text Input\r\n ENT = ENTER > = Command\r\n ------------\r\n>");
           p=0;
         } else if( isHex(cmd[0]) )
         {
           cmdType=CMD_FIRE_BOTH;
         } else if ( cmd[0] == '%' )
         {
           ptxt("\rYou don't need to type % for user+pass, just type the account number.\r\n>");
           p=0;
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
           ptxt("[unknown]\r\n>");
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
             fireEntry(CMD_FIRE_BOTH, strToHex(cmd), 0 );
             p=0;
           }
         } else if( cmdType==CMD_EXTENDED )
         {
           switch(cmd[1])
           {
             case 'a':
               newAccount(-1);
             break;
             case 'm':
               newMacro(-1);
             break;
             case 'f':
               cls();
               ptxt("WARNING: DESTROYS ALL DATA!\r\nAre you sure [y/n] ?");
               if(getOneChar() == 'y' && btnWait(BTN_TIMEOUT_IMPORTANT) )
               {
                 ES.lock();
                 ptxt("\r\nCurrent psw:");
                 if( login(0) )
                 {
                   ptxt("\r\n");
                   format();
                 } else {
                   ptxt("\r\nWrong password\r\n[lock]");
                   delay(1000);
                   return;
                 }
               } else {
                ptxtln("\r\n[abort]"); 
               }
             break;
             case 'p':
               changePass();
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
             case 'u':
               txt('%');
               setMacro();
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
           
           Serial.print(F("\r\n>"));
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
           } else if( cmd[1] == 13 )
           {
             search(CMD_FIRE_BOTH);
           } else if( cmd[1] == 's' )
           {
             search(CMD_SHOW_BOTH);
           } else {
             ptxt("\r\n[abort]\r\n>");
           }
         } else if( cmdType == CMD_ROBOT )
         {
           switch( cmd[1] )
           {
             case 'i':
               ptxtln("[RDY]");
               if( btnWait(BTN_TIMEOUT_IMPORTANT) )
               {
                 ptxtln("[OK]");
                 ES.importData();
                 return;
               } else {
                 ptxtln("[NO]");
               }
               p=0;
             break;
             case 'e':
             ptxtln("[RDY]");
             if( btnWait(BTN_TIMEOUT_IMPORTANT) )
             {
               ptxtln("[OK]");
               ES.exportData();
             } else {
               ptxtln("[NO]");
             }
             break;
             case 'l': //List for machines
             machineList();
             break;
           }
         }

         p=0;
       } //Second character 
       btnCoolDown=500;
       
      } //Incoming char
      
      //We detect macro btn press here
      if( !digitalRead(btnPin) && btnCoolDown == 0 )
      {
        btnCoolDown=20;
        if( macroNum != -1 )
        {
          fireEntry(CMD_FIRE_BOTH,macroNum, 1);
        } else {
          ptxtln("\r\n[not set]");
        }
      } else if(btnCoolDown!=0)
      {
        btnCoolDown--;
      }
      
    } //Serial connected

  } //Device unlocked
  
  
  //Turn off power to EEPROM
  I2E.power(LOW);

}

