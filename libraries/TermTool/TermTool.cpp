#include "TermTool.h"

void freeRam () {
  extern int __heap_start, *__brkval; 
  int v;
  Serial.print(F("\r\nFree:"));
  Serial.print( ((int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval)) );
  Serial.print("\r\n");
}


// Save some bytes by making often printed strings into their own functions.
void printAbort()
{
  ptxtln("\r\n[abort]");
}

void printError()
{
  ptxtln("[error]");  
}

void printOk()
{
  ptxtln("\r\n[done]");
}

void printBusy()
{
  ptxtln("\r[working]");
}