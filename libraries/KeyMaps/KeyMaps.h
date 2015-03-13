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
#define KBMAP_DEPC 7
#define KBMAP_DEMAC 8
#define KBMAP_SEPC 9
#define KBMAP_SEMAC 10

// To save flash, include only 4 layouts.
// Edit these to include layouts, valid choices are the above.
#define KBMAP_A KBMAP_USPC
#define KBMAP_B KBMAP_DKPC
#define KBMAP_C KBMAP_DKMAC
//#define KBMAP_D KBMAP_SEMAC

#include "keymap-us.h"
#include "keymap-dk.h"
#include "keymap-fr.h"
#include "keymap-de.h"
#include "keymap-se.h"

#define INVALID_KEYBOARD_LAYOUT 200


extern KeyMaps kbmaps;

#endif