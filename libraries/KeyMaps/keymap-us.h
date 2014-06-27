// My tests indicate that US ISO keyboards for mac are identical to the PC keyboards.


#if KBMAP_A == KBMAP_USPC || KBMAP_B == KBMAP_USPC || KBMAP_C == KBMAP_USPC || KBMAP_D == KBMAP_USPC ||  KBMAP_A == KBMAP_USMAC || KBMAP_B == KBMAP_USMAC || KBMAP_C == KBMAP_USMAC || KBMAP_D == KBMAP_USMAC

#define KBMAP_USPC_PTR &USPC_asciimap
#define KBMAP_USMAC_PTR &USPC_asciimap

#define KBMAP_USMAC_INUSE
#define KBMAP_USPC_INUSE


#if KBMAP_A == KBMAP_USPC
#define KBMAP_A_NAME "  1. USPC"
#endif

#if KBMAP_B == KBMAP_USPC
#define KBMAP_B_NAME "  2. USPC"
#endif

#if KBMAP_C == KBMAP_USPC
#define KBMAP_C_NAME "  3. USPC"
#endif

#if KBMAP_D == KBMAP_USPC
#define KBMAP_D_NAME "  4. USPC"
#endif

#if KBMAP_A == KBMAP_USMAC
#define KBMAP_A_NAME "  1. USMAC"
#endif

#if KBMAP_B == KBMAP_USMAC
#define KBMAP_B_NAME "  2. USMAC"
#endif

#if KBMAP_C == KBMAP_USMAC
#define KBMAP_C_NAME "  3. USMAC"
#endif


#if KBMAP_D == KBMAP_USMAC
#define KBMAP_D_NAME "  4. USMAC"
#endif


#endif