#ifndef TERMTOOL_H_INCLUDED
#define TERMTOOL_H_INCLUDED
#include "Arduino.h"
#define txtln(str) Serial.println(str)
#define ptxtln(str) Serial.println(F(str))
#define txt(str) Serial.print(str)
#define ptxt(str) Serial.print(F(str))
#define ptxtu(str) txtUnderline(F(str))
void freeRam();
#endif