#define _BSD_SOURCE	// Shut up about cfmakeraw
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <stdint.h>
#include<sys/stat.h>

struct termios tty;
struct termios tty_old;


uint8_t crc8(const uint8_t *addr, uint8_t len)
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

void fkLogin( char* psw, int USB, char* buf  )
{
  int lame=0;
    printf("Connecting to Final Key.\n");
    while( read(USB, buf,1) )
    {
	if( buf[0]=='#' )
	{
	  break;
	}
    }
    
    printf("Done.\n\nPress button to login!\n");
    while( read(USB, buf,1) )
    {
	if( buf[0]=='s' && (lame == 0 || lame==1)  )
	{
	  lame++;
	} else
	if( buf[0]==':' && lame == 2  )
	{
	  break;
	} else {
	  lame=0;
	}
	
    }
    
    printf("Signing in.\n");
    write( USB, psw, strlen(psw) );
    memset(psw,0,sizeof(psw));

    printf("Waiting for prompt.\n");
    while( read(USB, buf,1) )
    {
	if( buf[0]=='>' )
	{
	  break;
	}
    }
    printf("Got prompt.\n");
}

int getReady(int USB, char* buf, char* msg)
{
    int lame=0;
    while( read(USB, buf,1) )
    {
	if( buf[0] == '[' && lame == 0 )
	{
	  lame++;
	} else
	if( buf[0] == 'R' && lame == 1 )
	{
	  lame++;
	} else
	if( buf[0] == 'D' && lame == 2 )
	{
	  lame++;
	} else
	if( buf[0] == 'Y' && lame == 3 )
	{
	  lame++;
	} else
	if( buf[0] == ']' && lame == 4 )
	{
	  break;
	}  else
	{
	  lame=0;
	}

    }    
    printf(msg);
    
    lame=0;
    while( read(USB, buf,1) )
    {
	if( buf[0] == '[' && lame == 0 )
	{
	  lame++;
	} else
	if( buf[0] == 'O' && lame == 1 )
	{
	  lame++;
	} else
	if( buf[0] == 'N' && lame == 1 ) //Catch the 'N' in NO and abort mission.
	{
	  printf("Timed out!\nUnplug and replug and try again...\n");
	  return(0);
	} else 
	if( buf[0] == 'K' && lame == 2 )
	{
	  lame++;
	} else
	if( buf[0] == ']' && lame == 3 )
	{
	  lame++;
	} else
	if( buf[0] == '\r' && lame == 4 )
	{
	  lame++;
	} else
	if( buf[0] == '\n' && lame == 5 )
	{
	  break;
	} else
	{
	  lame=0;
	}
    }

    return(1);
}


void quit(int USB,FILE* fd, int status )
{
  if( USB )
  {
    close(USB);
  }
  if( fd )
  {
    fclose(fd);
  }
  exit(status);
}

int main(int argc, char** argv)
{
  //Don't worry, the following buffer sizes has been chosen completely at random and are not checked for overflows.
  char buf[100000];
  char devFile[1024];  
  char psw[128];
  
  if( argc != 4  && argc != 3)
  {
    printf("Usage: ./finalkeybackup ACTION FILE [TTYDEVICE]\n"
	   "       ACTION - backup or restore\n"
	   "       FILE   - The file to export into or import from.\n"
	   "       TTYDEVICE - Device file, defaults to /dev/FinalKey (Optional)\n"
	   "       (The restore option overwrites all data on your final key!)\n"
	   "       (Without the password for the backed-up data, key = bricked!!)\n\n");
    return(1);
  }
  
  if( argc == 3 )
  {
    strcpy( devFile, "/dev/FinalKey" );
  } else {
    strcpy( devFile, argv[3] );
  }
  
   struct stat f_st;
   if( stat( devFile, &f_st ) != 0 )
   {
     printf("Error: File not found '%s'\n", devFile );
     return(1);
   }
 
  memset(psw,0,sizeof(psw));
  

     struct termios term, saveTerm;

    /* Retrieve current terminal settings, turn echoing off */
    if (tcgetattr(STDIN_FILENO, &term) == -1)
    {
      printf("Could not get termattrs.\n");
      return(1);
    }
    saveTerm = term;                          /* So we can restore settings later */
    term.c_lflag &= ~ECHO;                /* ECHO off, other bits unchanged */
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) == -1)
    {
      printf("Could not set termattrs.\n");
      return(1);
    }

    fflush(stdout);

  

  printf("[Current] FinalKey password:");
  fgets(psw, sizeof(psw), stdin);
  printf("\n");
  strcpy( buf, psw );
  sprintf( psw, "%s\n", buf );
  memset(buf,0,sizeof(buf));

  if (tcsetattr(STDIN_FILENO, TCSANOW, &saveTerm) == -1)
  {
    printf("Could not restore termattrs.\n");
    return(1);
  }

  
  int USB = open( devFile, O_RDWR| O_NOCTTY );

  memset (&tty, 0, sizeof tty);

  
  /* Error Handling */
  if ( tcgetattr ( USB, &tty ) != 0 )
  {
    printf("Could not connect to %s (Error %i from tcgetattr).\n", devFile, errno );
    quit(USB, NULL,  EXIT_FAILURE);
  }

  /* Save old tty parameters */
  tty_old = tty;

  /* Set Baud Rate */
  cfsetospeed (&tty, (speed_t)B9600);
  cfsetispeed (&tty, (speed_t)B9600);

  /* Setting other Port Stuff */
  tty.c_cflag     &=  ~PARENB;        // Make 8n1
  tty.c_cflag     &=  ~CSTOPB;
  tty.c_cflag     &=  ~CSIZE;
  tty.c_cflag     |=  CS8;

 // tty.c_cflag     &=  ~CRTSCTS;       // no flow control
  tty.c_cc[VMIN]      =   0; //was 1      0 makes sense for reading from dev            // read doesn't block
  tty.c_cc[VTIME]     =   0; // was 5     ^^^            // 0.5 seconds read timeout
  //tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

  /* Make raw */
  cfmakeraw(&tty);

  /* Flush Port, then applies attributes */
  tcflush( USB, TCIFLUSH );
  if ( tcsetattr ( USB, TCSANOW, &tty ) != 0)
  {
    printf("Error %i from tcsetattr.\n", errno );
    quit(USB,NULL, EXIT_FAILURE);
  }

    int i=0;
    int lame=0;
  ///Write
  uint8_t crc;
  if( strcmp( argv[1], "restore" ) == 0 )
  {

    FILE* in = fopen( argv[2], "rb" );
    if(!in)
    {
      printf("Could not open file %s for reading, aborting.\n", argv[2]);
      quit(USB,NULL, EXIT_FAILURE);
    } else {
      printf("Opened: Writing to device..\n");
    }
    
    fkLogin(psw, USB, buf);
    
    printf("Requesting restore.\n");
    write( USB, "X",1 );
    write( USB, "i", 1 );
    printf("Waiting for RDY.\n\nWarning: Point of no return.\n         All data on the key will be overwritten!\n\n");

    if( !getReady(USB, buf, "Press button now to allow writing data to device.\n") )
    {
      quit(USB,NULL, EXIT_FAILURE);
    }

    printf("Writing data.\n");
    int run=1;
    lame=0;
    while( run )
    {
	while( read(USB, &buf,1) && run )
	{
	  if( buf[0]=='W' || buf[0] == 'O') // 'W'ait or byte-received 'O'kay
	  {
	    // nop, just wait for device...
	  } else if( buf[0]=='F' )
	  {
	    printf("\nFailure.\n");
	  } else if( buf[0]=='R' )
	  {
	    lame+=32;
	    printf("\rWrite: %i/64000 ",lame);
	    fflush(stdout);
	    fread( &buf, 1,32, in );
	    write( USB, &buf, 32 );
	    crc = fread( &buf, 1,1, in );
	    write( USB, &buf, 1 ); //Since the CRC was already verified when we recorded it, we simply use the one from the file instead of calculating it again.
	  } else if( buf[0]=='E' )
	  {
	    printf("\nDevice reports done.\n");
	    run=0;
	  } else if( buf[0] == 'C' )
	  {
	    printf("\nDevice reports CRC error.\nFormat device and try again.\n");
	    run=0;
	  } else {
	    printf("\nError:Got unrecognized byte value: 0x%X\n", buf[0]);
	  }
      }
	
	printf("\nFinished, exiting.\nUnplug and replug before using.\n");
	run=0;
    }
      quit(USB,in, EXIT_SUCCESS);
  }

  
  if( strcmp( argv[1], "backup" )==0 )
  {
    FILE* out = fopen(argv[2], "wb" );
    if( !out )
    {
      printf("Could not open file %s for writing, aborting.\n", argv[2]);
      quit(USB,NULL,EXIT_FAILURE);
    }
    int n = 0;

    fkLogin( psw, USB, buf );

    write( USB, "X",1 );
    write( USB, "e", 1 );
    
    if(!getReady(USB, buf, "\nPress button to allow reading data from device.\n"))
    {
      close(USB);
      return(1);
    }
    
    printf("Reading from device.\n");
    
    
    int b=0;
    uint8_t packet[32];
    
    lame=0;
    int state=0;
    while(read( USB, &buf, 1 ) > 0)
    {
      if( state == 0 )
      {
	if( lame== 0 && buf[0] == '[' )
	{
	  lame++;
	} else
	if( lame== 1 && buf[0] == 'B' )
	{
	  lame++;
	} else
	if( lame== 2 && buf[0] == 'E' )
	{
	  lame++;
	} else
	if(  lame== 3 && buf[0] == 'G' )
	{
	  lame++;
	} else
	if( lame== 4 && buf[0] == 'I' )
	{
	  lame++;
	} else
	if( lame== 5 && buf[0] == 'N' )
	{
	  lame++;
	} else
	if( lame== 6 && buf[0] == ']' )
	{
	  state++;
	  lame=0;
	  i=0;
	  printf("Got begin sequence.\n");
	} else {
	    printf("\nError:Got unrecognized byte value: 0x%X\n", buf[0]);
	  quit(USB,out,EXIT_FAILURE);

	}
      } else
      if( state == 1 )
      {
	i++;
	printf("\rReading byte %i/66000 ", i);
	fwrite( buf, 1,1, out );
	fflush( out );
	
	if(b<32)
	{
	  packet[b] = buf[0];
	}
	b++;

	if(b==33)
	{
	  if( (uint8_t)buf[0] != crc8(packet, 32) )
	  {
	    printf("\nError: CRC check failed at byte %i.\nUnplug and replug and try again.\n");
	    quit(USB,out,EXIT_FAILURE);
	  }
	  b=0;
	}
	
	
	if( i == 66000 )
	{
	  	state++;
		lame=0;
		printf("\nFinished, waiting for end sequence.\n");
	}
      } else
      if( state == 2 )
      {
	if( lame == 0 && buf[0] == '[' )
	{
	  lame++;
	} else
	if( lame == 1 && buf[0] == 'E' )
	{
	  lame++;
	} else
	if( lame== 2 && buf[0] == 'N' )
	{
	  lame++;
	} else
	if(  lame== 3 && buf[0] == 'D' )
	{
	  lame++;
	} else
	if( lame== 4 && buf[0] == ']' )
	{
	  printf("Got end sequence.\n");
	  break;
	} else {
	    printf("\nError:Got unrecognized byte value: 0x%X\n", buf[0]);
	  quit(USB,out,EXIT_FAILURE);
	}
      }
      
    }
    printf("\nFile saved.\n");
    quit(USB,out,EXIT_SUCCESS);
  }
   quit(USB,NULL,EXIT_FAILURE);
}
