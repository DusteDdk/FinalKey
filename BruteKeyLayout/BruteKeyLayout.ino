
//I've not been able to find any nice list of "usb scancode to ascii" map, so this tool will do most of the nasty work, leaving few characters to be edited manually.


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


#define BTNA 9
#define BTNB 8

void setup() {
  Serial.begin(9600);
  

  pinMode(BTNA, INPUT); // set pin to input
  digitalWrite(BTNA, HIGH);  //Enable internal pullups for button

  pinMode(BTNB, INPUT); // set pin to input
  digitalWrite(BTNB, HIGH);  //Enable internal pullups for button

}

#define SHIFT 2
//Calculated as (1<<(KEY_RIGHT_ALT-128))
#define ALT 64


uint8_t btn( uint8_t b )
{
  if( !digitalRead( b ) )
  {
    while( !digitalRead( b ) )
    {
      delay(20);
    }
    return(1);
  }
  return(0);
}

uint8_t tryCode( char tar, uint8_t sc, uint8_t mod )
{
  clearSerialInput();
  
  //Type it raw
  Keyboard.raw( sc, mod );
  
  //Wait
  delay(20);
  
  //Try reading it back
  
  if(Serial.available())
  {
    if( (uint8_t)tar == Serial.read() )  
    {
      Serial.print("Found! ( 0x"); Serial.print( sc, HEX); Serial.print(", 0x"); Serial.print( mod, HEX ); Serial.print(" )\r\n");
      return(1);
    }
  } else {
    Keyboard.raw(0x2a,0); //backspace
    delay(20);
  }

    clearSerialInput();


  return(0);
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

uint8_t printable( int i )
{
  int c;
  for(c =0 ; c < 92; c++ ) 
  {
    if( passChars[c] == (uint8_t)i )
    {
       return(1);
    }
  }  return(0);
}

void testSequence()
{
  int i;
  Serial.print("\r\nTest seq layout?\r\n  00 = DK_PC\r\n  01 = DK_MAC\r\n  02 = US_PC\r\n  03 = US_MAC\r\n  04 = FR_PC\r\n  05 = FR_MAC\r\n ");


  int l=-1;
  while( l < 0 || l > 100 )
  {
   l=collectNum();
   Serial.print("\r\n");
  }
  

  

  Keyboard.begin(l);
  
  Serial.print("Ready.\r\n#");

  while(!btn(BTNA))
  {
  }
  
    //Test keyboard: -->
  for(i =0; i < 92; i++)
  {
    Keyboard.write( passChars[i] );
    Serial.write( (char)passChars[i] );
  }
  //// <-- Test
  Serial.println();
  
}

void loop() {

  uint8_t asciiMap[256];
  
  int i;
    while( !Serial );
  for(i = 0; i < 256; i ++ )
  {
    asciiMap[i] = 0;
  }



  
  int sc;
  Serial.print("Try code (hex) (ff for test seq):");
  sc=collectNum();
  if( sc!=-1 && sc!= 0xff)
  {
    
    Serial.print("\r\nMods (01 = none, 02 = shift, 03 = alt, 04 = alt+shift):");
    int mod = collectNum();
    int m=0;
    switch(mod)
    {
      case 1:
      
      break;
      case 2:
       m=SHIFT;
      break;
      case 3:
       m=ALT;
      break;
      case 4:
       m=SHIFT|ALT;
      break;
      default:
        Serial.print("Error2\r\n");
      break;
     }
     
     Serial.print("\r\nReady to type...\r\n");
       while(!btn(BTNA))
  {
  }
     Keyboard.raw( sc, m );
     Serial.print("\r\nDone\r\n");
    
    
  } else if( sc=0xff)
  {
    testSequence();
  }else{
    Serial.print("Error1\r\n");
  }
  
  /*
  int sc;

  for( i=0; i < 92; i++)
 {
  Serial.print("Looking for ");
  Serial.write( passChars[i] );
  Serial.print(": ");
  
  //For each scancode
  
  for(sc=0x04; sc < 0x39; sc++)
  {
  
    //Without mods
      if( tryCode( passChars[i], sc, 0 ) )
      {
        asciiMap[2*passChars[i]] = sc;
        asciiMap[2*passChars[i]+1] = 0;
        break;
      } else if( tryCode( passChars[i], sc, SHIFT ) )
      {
        asciiMap[2*passChars[i]] = sc;
        asciiMap[2*passChars[i]+1] = SHIFT;
        break;
      }

    }

    if( sc == 0x39 )
    {
        asciiMap[2*passChars[i]] = 0xff;
        asciiMap[2*passChars[i]+1] = 0xff;

      Serial.print("Failed.\r\n");
    }
  
    
   
  
  }
  
  Serial.print("\r\n");
  
  for( i=0; i < 256; i+=2 )
  {
    Serial.print( "	0x");
    if(asciiMap[i] < 0x0F )
    {
      Serial.print('0');
    }
    Serial.print( asciiMap[i], HEX );
    Serial.print( ", " );
    if( asciiMap[i+1] == 0 )
    {
      Serial.print("NOMOD");
    } else if( asciiMap[i+1] == SHIFT )
    {
      Serial.print("SHIFT");
    } else if( asciiMap[i+1] == ALT )
    {
      Serial.print("ALT");
    } else {
      Serial.print(">FAIL<");
    }
    Serial.print(", // ");
    if( printable(i/2) )
    {
      Serial.print( (char)(i/2) );
    } else {
      Serial.print( " 0x" );
      
      Serial.print( (i/2), HEX);
    }
    Serial.print("\r\n");
  }
  Serial.print("\r\n");*/
  
}

