#ifndef KEYMAPS_H
#define KEYMAPS_H
#include <Arduino.h>
#include <TermTool.h>

class KeyMaps
{
  public:
       void setKbMap(uint8_t keyMap);
};

#define NOMOD 0x00

//Calculated as (1<<(KEY_LEFT_SHIFT-128))
#define SHIFT 2
//Calculated as (1<<(KEY_RIGHT_ALT-128))
#define ALT 64

#define KBMAP_USPC 1
#define KBMAP_USMAC 2
#define KBMAP_DKPC 3
#define KBMAP_DKMAC 4
#define KBMAP_FRPC 5
#define KBMAP_FRMAC 6

// To save flash, include only 4 layouts.
// Edit these to include layouts, valid choices are the above.
#define KBMAP_A KBMAP_USPC
#define KBMAP_B KBMAP_DKPC
#define KBMAP_C KBMAP_DKMAC
#define KBMAP_D KBMAP_FRPC

#include "keymap-us.h"
#include "keymap-dk.h"
#include "keymap-fr.h"

#define INVALID_KEYBOARD_LAYOUT 200

extern KeyMaps kbmaps;

#endif