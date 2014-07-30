// PC
#if KBMAP_A == KBMAP_DEPC || KBMAP_B == KBMAP_DEPC || KBMAP_C == KBMAP_DEPC || KBMAP_D == KBMAP_DEPC

#define KBMAP_DEPC_PTR &DEPC_asciimap

#define KBMAP_DEPC_INUSE

#if KBMAP_A == KBMAP_DEPC
#define KBMAP_A_NAME "  1. DEPC"
#endif

#if KBMAP_B == KBMAP_DEPC
#define KBMAP_B_NAME "  2. DEPC"
#endif

#if KBMAP_C == KBMAP_DEPC
#define KBMAP_C_NAME "  3. DEPC"
#endif

#if KBMAP_D == KBMAP_DEPC
#define KBMAP_D_NAME "  4. DEPC"
#endif

#endif

// MAC

#if KBMAP_A == KBMAP_DEMAC || KBMAP_B == KBMAP_DEMAC || KBMAP_C == KBMAP_DEMAC || KBMAP_D == KBMAP_DEMAC

#define KBMAP_DEMAC_PTR &DEMAC_asciimap

#define KBMAP_DEMAC_INUSE

#if KBMAP_A == KBMAP_DEMAC
#define KBMAP_A_NAME "  1. DEMAC"
#endif

#if KBMAP_B == KBMAP_DEMAC
#define KBMAP_B_NAME "  2. DEMAC"
#endif

#if KBMAP_C == KBMAP_DEMAC
#define KBMAP_C_NAME "  3. DEMAC"
#endif

#if KBMAP_D == KBMAP_DEMAC
#define KBMAP_D_NAME "  4. DEMAC"
#endif

#endif