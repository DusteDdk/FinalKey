// French Keyboard layout, thanks to Christophe for providing support and pictures.

// *-- OSX NOTE --*
// A note to mac users: This is the ISO keyboard scancodes not ANSI or Japanese,
// To change keyboard type, unplug FinalKey, rm /Library/Preferences/com.apple.keyboardtype.plist and reboot
// After rebooting, replug FinalKey and press "g" at the "type key next to shift" dialog, it will fail detection,
// that is good, as it allows you to continue to manual selection, where you choose ISO and become happy.

#if KBMAP_A == KBMAP_FRPC || KBMAP_B == KBMAP_FRPC || KBMAP_C == KBMAP_FRPC || KBMAP_D == KBMAP_FRPC

#define KBMAP_FRPC_PTR &FRPC_asciimap
#define KBMAP_FRPC_INUSE


#if KBMAP_A == KBMAP_FRPC
#define KBMAP_A_NAME "  1. FRPC"
#endif

#if KBMAP_B == KBMAP_FRPC
#define KBMAP_B_NAME "  2. FRPC"
#endif

#if KBMAP_C == KBMAP_FRPC
#define KBMAP_C_NAME "  3. FRPC"
#endif

#if KBMAP_D == KBMAP_FRPC
#define KBMAP_D_NAME "  4. FRPC"
#endif

#endif


#if KBMAP_A == KBMAP_FRMAC || KBMAP_B == KBMAP_FRMAC || KBMAP_C == KBMAP_FRMAC || KBMAP_D == KBMAP_FRMAC

#define KBMAP_FRMAC_PTR &FRMAC_asciimap
#define KBMAP_FRMAC_INUSE

#if KBMAP_A == KBMAP_FRMAC
#define KBMAP_A_NAME "  1. FRMAC"
#endif

#if KBMAP_B == KBMAP_FRMAC
#define KBMAP_B_NAME "  2. FRMAC"
#endif

#if KBMAP_C == KBMAP_FRMAC
#define KBMAP_C_NAME "  3. FRMAC"
#endif

#if KBMAP_D == KBMAP_FRMAC
#define KBMAP_D_NAME "  4. FRMAC"
#endif

#endif