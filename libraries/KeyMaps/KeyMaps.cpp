#include "KeyMaps.h"

#include "keymap-us.inc"
#include "keymap-dk.inc"
#include "keymap-fr.inc"


void KeyMaps::setKbMap(uint8_t mapNum)
{
  switch(mapNum)
  {
#ifdef KBMAP_USPC_PTR
    case KBMAP_USPC:
      Keyboard.begin( (uint8_t*)(KBMAP_USPC_PTR) );
      ptxtln("[Keyboard: US-PC]");
    break;
#endif

#ifdef KBMAP_USMAC_PTR
    case KBMAP_USMAC:
      Keyboard.begin( (uint8_t*)(KBMAP_USMAC_PTR) );
      ptxtln("[Keyboard: US-MAC]");
    break;
#endif

#ifdef KBMAP_DKPC_PTR
    case KBMAP_DKPC:
      Keyboard.begin( (uint8_t*)(KBMAP_DKPC_PTR) );
      ptxtln("[Keyboard: DK-PC]");
    break;
#endif
#ifdef KBMAP_DKMAC_PTR
    case KBMAP_DKMAC:
      Keyboard.begin( (uint8_t*)(KBMAP_DKMAC_PTR) );
      ptxtln("[Keyboard: DK-MAC]");
    break;
#endif

#ifdef KBMAP_FRPC_PTR
    case KBMAP_FRPC:
      Keyboard.begin( (uint8_t*)(KBMAP_FRPC_PTR) );
      ptxtln("[Keyboard: FR-PC]");
    break;
#endif
#ifdef KBMAP_FRMAC_PTR
    case KBMAP_FRMAC:
      Keyboard.begin( (uint8_t*)(KBMAP_FRMAC_PTR) );
      ptxtln("[Keyboard: FR-MAC]");
    break;
#endif
    default:
      ptxtln("[Keyboard: Unknown]");
    break;
  }
}

KeyMaps kbmaps = KeyMaps();
