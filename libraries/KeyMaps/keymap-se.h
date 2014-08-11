// PC
#if KBMAP_A == KBMAP_SEPC || KBMAP_B == KBMAP_SEPC || KBMAP_C == KBMAP_SEPC || KBMAP_D == KBMAP_SEPC

#define KBMAP_SEPC_PTR &SEPC_asciimap

#define KBMAP_SEPC_INUSE

#if KBMAP_A == KBMAP_SEPC
#define KBMAP_A_NAME "  1. SEPC"
#endif

#if KBMAP_B == KBMAP_SEPC
#define KBMAP_B_NAME "  2. SEPC"
#endif

#if KBMAP_C == KBMAP_SEPC
#define KBMAP_C_NAME "  3. SEPC"
#endif

#if KBMAP_D == KBMAP_SEPC
#define KBMAP_D_NAME "  4. SEPC"
#endif

#endif

// MAC

#if KBMAP_A == KBMAP_SEMAC || KBMAP_B == KBMAP_SEMAC || KBMAP_C == KBMAP_SEMAC || KBMAP_D == KBMAP_SEMAC

#define KBMAP_SEMAC_PTR &SEMAC_asciimap

#define KBMAP_SEMAC_INUSE

#if KBMAP_A == KBMAP_SEMAC
#define KBMAP_A_NAME "  1. SEMAC"
#endif

#if KBMAP_B == KBMAP_SEMAC
#define KBMAP_B_NAME "  2. SEMAC"
#endif

#if KBMAP_C == KBMAP_SEMAC
#define KBMAP_C_NAME "  3. SEMAC"
#endif

#if KBMAP_D == KBMAP_SEMAC
#define KBMAP_D_NAME "  4. SEMAC"
#endif

#endif