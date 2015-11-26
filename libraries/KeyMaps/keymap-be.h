// Belgian Keyboard layout, thanks to Jacques for pictures


#if KBMAP_A == KBMAP_BEPC || KBMAP_B == KBMAP_BEPC || KBMAP_C == KBMAP_BEPC || KBMAP_D == KBMAP_BEPC

#define KBMAP_BEPC_PTR &BEPC_asciimap
#define KBMAP_BEPC_INUSE


#if KBMAP_A == KBMAP_BEPC
#define KBMAP_A_NAME "  1. BEPC"
#endif

#if KBMAP_B == KBMAP_BEPC
#define KBMAP_B_NAME "  2. BEPC"
#endif

#if KBMAP_C == KBMAP_BEPC
#define KBMAP_C_NAME "  3. BEPC"
#endif

#if KBMAP_D == KBMAP_BEPC
#define KBMAP_D_NAME "  4. BEPC"
#endif

#endif


#if KBMAP_A == KBMAP_BEMAC || KBMAP_B == KBMAP_BEMAC || KBMAP_C == KBMAP_BEMAC || KBMAP_D == KBMAP_BEMAC

#define KBMAP_BEMAC_PTR &BEMAC_asciimap
#define KBMAP_BEMAC_INUSE

#if KBMAP_A == KBMAP_BEMAC
#define KBMAP_A_NAME "  1. BEMAC"
#endif

#if KBMAP_B == KBMAP_BEMAC
#define KBMAP_B_NAME "  2. BEMAC"
#endif

#if KBMAP_C == KBMAP_BEMAC
#define KBMAP_C_NAME "  3. BEMAC"
#endif

#if KBMAP_D == KBMAP_BEMAC
#define KBMAP_D_NAME "  4. BEMAC"
#endif

#endif