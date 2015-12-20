// PC
#if KBMAP_A == KBMAP_EEPC || KBMAP_B == KBMAP_EEPC || KBMAP_C == KBMAP_EEPC || KBMAP_D == KBMAP_EEPC

#define KBMAP_EEPC_PTR &EEPC_asciimap

#define KBMAP_EEPC_INUSE

#if KBMAP_A == KBMAP_EEPC
#define KBMAP_A_NAME "  1. EEPC"
#endif

#if KBMAP_B == KBMAP_EEPC
#define KBMAP_B_NAME "  2. EEPC"
#endif

#if KBMAP_C == KBMAP_EEPC
#define KBMAP_C_NAME "  3. EEPC"
#endif

#if KBMAP_D == KBMAP_EEPC
#define KBMAP_D_NAME "  4. EEPC"
#endif

#endif

// MAC

#if KBMAP_A == KBMAP_EEMAC || KBMAP_B == KBMAP_EEMAC || KBMAP_C == KBMAP_EEMAC || KBMAP_D == KBMAP_EEMAC

#define KBMAP_EEMAC_PTR &EEMAC_asciimap

#define KBMAP_EEMAC_INUSE

#if KBMAP_A == KBMAP_EEMAC
#define KBMAP_A_NAME "  1. EEMAC"
#endif

#if KBMAP_B == KBMAP_EEMAC
#define KBMAP_B_NAME "  2. EEMAC"
#endif

#if KBMAP_C == KBMAP_EEMAC
#define KBMAP_C_NAME "  3. EEMAC"
#endif

#if KBMAP_D == KBMAP_EEMAC
#define KBMAP_D_NAME "  4. EEMAC"
#endif

#endif