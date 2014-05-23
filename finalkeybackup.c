/*
 * License: WTFPL ( http://www.wtfpl.net/ )
 * Backup tool for creating and restoring backups.
 * The output file is identical to the encrypted eeprom, except for one
 * additional byte crc8 checksum every 32 bytes.
 * Even though the output file is encrypted, it is strongly adviced to store
 * this file in a safe place, such of a dedicated USB stick/memory card or
 * burn it to another Final Key before deleting the file.
 * 
 * An example output of such a backup file is available for download at:
 * http://cyberstalker.dk/finalkey/hacking/ - At the time of committing this,
 * that file is not known to have been broken, so at least some security can be
 * assumed.
 * 
 * This utility may result in loss of data in the following ways:
 * 	You override an existing backup file.
 * 	You override a Final Key which contained data that was not backed-up.
 * 	You restore a Final Key with an corrupt/invalid key file.
 * 	You restore a backup with another firmware identifier.
 * 		At the time of writing [FinalKey2] is the correct identifier.
 * 	You're generally careless/not knowing what you're doing.
 * 
 * 
 * This utility may brick your device in the following ways:
 * 	You restore an image to which you do not know the password.
 * 	The restore-operation is interrupted at an unlucky time.
 * 		(After writing byte 13, before writing byte 124,
 * 		and then only if restoring an image with different password).
 * 
 * 
 * Creating backups (backing up your Final Key):
 *	You need to know one password, the one which unlocks the key.
 * 	./fkbck.bin backup mybackup.bin
 * 	That saves your backup to the file "mybackup.bin" I suggest you
 * 	come up with a more fitting name, maybe including the date.
 * 
 * Restoring backups (writing a backup to a Final Key):
 * 	You need to know two passwords, which may be identical:
 * 		The password which unlocks the key that you wish to overwrite.
 * 		The password which unlocks the image that you wish to restore.
 * 	If you do not know the password which unlocks the key, you can't begin
 * 	the restore-process, and if you do not know the password which unlocks
 * 	the image that you write to the key, you won't be able to unlock it.
 * 	./fkbck.bin restore mybackup.bin
 * 	This writes mybackup.bin onto the key, all existing data on that key
 * 	will be overwritten. So think about it.
 * 	
 * How to build:
 * 	To compile, you need a C compiler, and the standard and posix libs.
 *
 * 	gcc --std=c99 -o ./fkbck.bin ./finalkeybackup.c 
 * 
 * With heavy heart and much shame, follows a late night writing by
 * Jimmy Christensen, this could be done better, faster and smarter and safer,
 * but I decided to stop when it worked, sorry about that, if you want to,
 * improve it and send me a pull-request on github.
 * 
 */

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
    printf("Usage: finalkeybackup ACTION FILE [TTYDEVICE]\n"
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
   
   
   //We want to make sure we start with a known state
   //So we force people to disconnect and reconnect it.
   if( lstat( devFile, &f_st ) == 0 )
   {
      //Seems like a good place to check if anyone has opened the device already.
      //We don't expect lsof or grep to be there, and if it's not, we still run.
      char cmdStr[128];
      char realPath[8192]; //Almost chosen randomly, 4096 seems to be an acceptable max path length so this should just fine.
      
      if( realpath( devFile, realPath ) == NULL )
      {
	printf("Error: Could not resolve '%s'\n", devFile);
	return(1);
      }
      
      sprintf( cmdStr, "lsof -n | grep %s 2> /dev/null", realPath );
      FILE* fpCmd = popen( cmdStr, "r" );
      if( fpCmd )
      {
	if(fgets(buf, 32, fpCmd) != NULL)
	{
	  printf("\nError: The Final Key is in use by another application.\n      Please close other applications and try again.\n");
	  pclose(fpCmd);
	  return(1);
	}
	
	pclose(fpCmd);
      } else {
	printf("It it advised that you install the 'lsof' package before using this program.\n");
      }

     printf("\nPlease unplug The Final Key now.\n");
     
     while( stat( devFile, &f_st ) == 0 )
     {
       sleep(1);
     }
     
     printf("Thank you, please connect The Final Key again.\n");
   } else {
     printf("Please connect The Final Key.\n");
   }    
   

  //Wait for user to insert the key
  int timeOut=30;
  while( stat( devFile, &f_st ) != 0 )
  {
    timeOut--;
    if(timeOut < 0)
    {
      printf("Error: Could not connect to %s\n", devFile);
      return(1);
    }
    sleep(1);
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
