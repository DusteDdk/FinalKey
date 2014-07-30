// PC
#if KBMAP_A == KBMAP_DKPC || KBMAP_B == KBMAP_DKPC || KBMAP_C == KBMAP_DKPC || KBMAP_D == KBMAP_DKPC

#define KBMAP_DKPC_PTR &DKPC_asciimap

#define KBMAP_DKPC_INUSE

#if KBMAP_A == KBMAP_DKPC
#define KBMAP_A_NAME "  1. DKPC"
#endif

#if KBMAP_B == KBMAP_DKPC
#define KBMAP_B_NAME "  2. DKPC"
#endif

#if KBMAP_C == KBMAP_DKPC
#define KBMAP_C_NAME "  3. DKPC"
#endif

#if KBMAP_D == KBMAP_DKPC
#define KBMAP_D_NAME "  4. DKPC"
#endif

#endif

// MAC

#if KBMAP_A == KBMAP_DKMAC || KBMAP_B == KBMAP_DKMAC || KBMAP_C == KBMAP_DKMAC || KBMAP_D == KBMAP_DKMAC

#define KBMAP_DKMAC_PTR &DKMAC_asciimap

#define KBMAP_DKMAC_INUSE

#if KBMAP_A == KBMAP_DKMAC
#define KBMAP_A_NAME "  1. DKMAC"
#endif

#if KBMAP_B == KBMAP_DKMAC
#define KBMAP_B_NAME "  2. DKMAC"
#endif

#if KBMAP_C == KBMAP_DKMAC
#define KBMAP_C_NAME "  3. DKMAC"
#endif

#if KBMAP_D == KBMAP_DKMAC
#define KBMAP_D_NAME "  4. DKMAC"
#endif

#endif