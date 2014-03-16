#include "TermTool.h"

void freeRam () {
  extern int __heap_start, *__brkval; 
  int v;
  Serial.print(F("\r\nFree:"));
  Serial.print( ((int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval)) );
  Serial.print("\r\n");
}