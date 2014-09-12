#include "KeyMaps.h"

#include "keymap-us.inc"
#include "keymap-dk.inc"
#include "keymap-fr.inc"
#include "keymap-de.inc"
#include "keymap-se.inc"


void KeyMaps::setKbMap(uint8_t mapNum)
{
  ptxt("[Keyboard: ");
  switch(mapNum)
  {
#ifdef KBMAP_USPC_PTR
    case KBMAP_USPC:
      Keyboard.begin( (uint8_t*)(KBMAP_USPC_PTR) );
      ptxtln("USPC]");
    break;
#endif

#ifdef KBMAP_USMAC_PTR
    case KBMAP_USMAC:
      Keyboard.begin( (uint8_t*)(KBMAP_USMAC_PTR) );
      ptxtln("USMAC]");
    break;
#endif

#ifdef KBMAP_DKPC_PTR
    case KBMAP_DKPC:
      Keyboard.begin( (uint8_t*)(KBMAP_DKPC_PTR) );
      ptxtln("DKPC]");
    break;
#endif
#ifdef KBMAP_DKMAC_PTR
    case KBMAP_DKMAC:
      Keyboard.begin( (uint8_t*)(KBMAP_DKMAC_PTR) );
      ptxtln("DKMAC]");
    break;
#endif

#ifdef KBMAP_FRPC_PTR
    case KBMAP_FRPC:
      Keyboard.begin( (uint8_t*)(KBMAP_FRPC_PTR) );
      ptxtln("FRPC]");
    break;
#endif
#ifdef KBMAP_FRMAC_PTR
    case KBMAP_FRMAC:
      Keyboard.begin( (uint8_t*)(KBMAP_FRMAC_PTR) );
      ptxtln("FRMAC]");
    break;
#endif
    
#ifdef KBMAP_DEPC_PTR
    case KBMAP_DEPC:
      Keyboard.begin( (uint8_t*)(KBMAP_DEPC_PTR) );
      ptxtln("DEPC]");
    break;
#endif
#ifdef KBMAP_DEMAC_PTR
    case KBMAP_DEMAC:
      Keyboard.begin( (uint8_t*)(KBMAP_DEMAC_PTR) );
      ptxtln("DEMAC]");
    break;
#endif
#ifdef KBMAP_SEPC_PTR
    case KBMAP_SEPC:
      Keyboard.begin( (uint8_t*)(KBMAP_SEPC_PTR) );
      ptxtln("SE-PC]");
    break;
#endif
#ifdef KBMAP_SEMAC_PTR
    case KBMAP_SEMAC:
      Keyboard.begin( (uint8_t*)(KBMAP_SEMAC_PTR) );
      ptxtln("SEMAC]");
    break;
#endif
    default:
      ptxtln("ERROR]");
    break;
  }

}

KeyMaps kbmaps = KeyMaps();
