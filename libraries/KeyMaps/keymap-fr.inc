#ifdef KBMAP_FRPC_INUSE

const uint8_t FRPC_asciimap[256] PROGMEM =
{
	0x00,NOMOD,    // NUL
	0x00,NOMOD,    // SOH
	0x00,NOMOD,    // STX
	0x00,NOMOD,    // ETX
	0x00,NOMOD,    // EOT
	0x00,NOMOD,    // ENQ
	0x00,NOMOD,    // ACK  
	0x00,NOMOD,    // BEL
	0x2a,NOMOD,    // BS	Backspace
	0x2b,NOMOD,    // TAB	Tab (2b)
	0x28,NOMOD,    // LF	Enter (28)
	0x00,NOMOD,    // VT 
	0x00,NOMOD,    // FF 
	0x00,NOMOD,    // CR 
	0x00,NOMOD,    // SO 
	0x00,NOMOD,    // SI 
	0x00,NOMOD,    // DEL
	0x00,NOMOD,    // DC1
	0x00,NOMOD,    // DC2
	0x00,NOMOD,    // DC3
	0x00,NOMOD,    // DC4
	0x00,NOMOD,    // NAK
	0x00,NOMOD,    // SYN
	0x00,NOMOD,    // ETB
	0x00,NOMOD,    // CAN
	0x00,NOMOD,    // EM 
	0x00,NOMOD,    // SUB
	0x00,NOMOD,    // ESC
	0x00,NOMOD,    // FS 
	0x00,NOMOD,    // GS 
	0x00,NOMOD,    // RS 
	0x00,NOMOD,    // US 
	0x2C, NOMOD, // ' ' 
        0x38, NOMOD, // !
        0x20, NOMOD, // "
        0x20, ALT,   // #
        0x30, NOMOD, // $
        0x34, SHIFT, // %
        0x1E, NOMOD, // &
        0x21, NOMOD, // '
        0x22, NOMOD, // (
        0x2D, NOMOD, // )
        0x31, NOMOD, // *
        0x2E, SHIFT, // +
        0x10, NOMOD, // ,
        0x23, NOMOD, // -
        0x36, SHIFT, // .
        0x37, SHIFT, // /
        0x27, SHIFT, // 0
        0x1E, SHIFT, // 1
        0x1F, SHIFT, // 2
        0x20, SHIFT, // 3
        0x21, SHIFT, // 4
        0x22, SHIFT, // 5
        0x23, SHIFT, // 6
        0x24, SHIFT, // 7
        0x25, SHIFT, // 8
        0x26, SHIFT, // 9
        0x37, NOMOD, // :
        0x36, NOMOD, // ;
	0x64, NOMOD, // <
        0x2E, NOMOD, // =
	0x64, SHIFT, // >
        0x10, SHIFT, // ?
        0x27, ALT  , // @
        0x14, SHIFT, // A
        0x05, SHIFT, // B
        0x06, SHIFT, // C
        0x07, SHIFT, // D
        0x08, SHIFT, // E
        0x09, SHIFT, // F
        0x0A, SHIFT, // G
        0x0B, SHIFT, // H
        0x0C, SHIFT, // I
        0x0D, SHIFT, // J
        0x0E, SHIFT, // K
        0x0F, SHIFT, // L
        0x33, SHIFT, // M
        0x11, SHIFT, // N
        0x12, SHIFT, // O
        0x13, SHIFT, // P
        0x04, SHIFT, // Q
        0x15, SHIFT, // R
        0x16, SHIFT, // S
        0x17, SHIFT, // T
        0x18, SHIFT, // U
        0x19, SHIFT, // V
        0x1D, SHIFT, // W
        0x1B, SHIFT, // X
        0x1C, SHIFT, // Y
        0x1A, SHIFT, // Z
        0x22, ALT,   // [
        0x25, ALT  , // bslash
        0x2d, ALT,   // ]
        0x00, NOMOD, //  0x5E
        0x25, NOMOD, // _
        0x00, NOMOD, //  0x60
        0x14, NOMOD, // a
        0x05, NOMOD, // b
        0x06, NOMOD, // c
        0x07, NOMOD, // d
        0x08, NOMOD, // e
        0x09, NOMOD, // f
        0x0A, NOMOD, // g
        0x0B, NOMOD, // h
        0x0C, NOMOD, // i
        0x0D, NOMOD, // j
        0x0E, NOMOD, // k
        0x0F, NOMOD, // l
        0x33, NOMOD, // m
        0x11, NOMOD, // n
        0x12, NOMOD, // o
        0x13, NOMOD, // p
        0x04, NOMOD, // q
        0x15, NOMOD, // r
        0x16, NOMOD, // s
        0x17, NOMOD, // t
        0x18, NOMOD, // u
        0x19, NOMOD, // v
        0x1D, NOMOD, // w
        0x1B, NOMOD, // x
        0x1C, NOMOD, // y
        0x1A, NOMOD, // z
        0x21, ALT,   // {
        0x23, ALT,   // |
        0x2e, ALT,   // }
        0x00, NOMOD, //  0x7E
        0x00, NOMOD, //  0x7F
};
#endif


#ifdef KBMAP_FRMAC_INUSE

const uint8_t FRMAC_asciimap[256] PROGMEM =
{
	0x00,NOMOD,    // NUL
	0x00,NOMOD,    // SOH
	0x00,NOMOD,    // STX
	0x00,NOMOD,    // ETX
	0x00,NOMOD,    // EOT
	0x00,NOMOD,    // ENQ
	0x00,NOMOD,    // ACK  
	0x00,NOMOD,    // BEL
	0x2a,NOMOD,    // BS	Backspace
	0x2b,NOMOD,    // TAB	Tab (2b)
	0x28,NOMOD,    // LF	Enter (28)
	0x00,NOMOD,    // VT 
	0x00,NOMOD,    // FF 
	0x00,NOMOD,    // CR 
	0x00,NOMOD,    // SO 
	0x00,NOMOD,    // SI 
	0x00,NOMOD,    // DEL
	0x00,NOMOD,    // DC1
	0x00,NOMOD,    // DC2
	0x00,NOMOD,    // DC3
	0x00,NOMOD,    // DC4
	0x00,NOMOD,    // NAK
	0x00,NOMOD,    // SYN
	0x00,NOMOD,    // ETB
	0x00,NOMOD,    // CAN
	0x00,NOMOD,    // EM 
	0x00,NOMOD,    // SUB
	0x00,NOMOD,    // ESC
	0x00,NOMOD,    // FS 
	0x00,NOMOD,    // GS 
	0x00,NOMOD,    // RS 
	0x00,NOMOD,    // US 
	0x2C, NOMOD, // ' ' 
        0x25, NOMOD, // ! (MacFix)
        0x20, NOMOD, // "
        0x35, SHIFT, // # (MacFix)
        0x30, NOMOD, // $
        0x34, SHIFT, // %
        0x1E, NOMOD, // &
        0x21, NOMOD, // '
        0x22, NOMOD, // (
        0x2D, NOMOD, // )
	0x55, NOMOD, // * (Fixed for mac, thank you microsoft (scancode.doc))
        0x38, SHIFT, // + (MacFix)
        0x10, NOMOD, // ,
        0x2E, NOMOD, // - (MacFix)
        0x36, SHIFT, // .
        0x37, SHIFT, // /
        0x27, SHIFT, // 0
        0x1E, SHIFT, // 1
        0x1F, SHIFT, // 2
        0x20, SHIFT, // 3
        0x21, SHIFT, // 4
        0x22, SHIFT, // 5
        0x23, SHIFT, // 6
        0x24, SHIFT, // 7
        0x25, SHIFT, // 8
        0x26, SHIFT, // 9
        0x37, NOMOD, // :
        0x36, NOMOD, // ;
	0x64, NOMOD, // < (MacFix)
        0x38, NOMOD, // = (MacFix)
	0x64, SHIFT, // > (Macfix)
        0x10, SHIFT, // ?
	0x35, NOMOD, // @ (MacFix)
        0x14, SHIFT, // A
        0x05, SHIFT, // B
        0x06, SHIFT, // C
        0x07, SHIFT, // D
        0x08, SHIFT, // E
        0x09, SHIFT, // F
        0x0A, SHIFT, // G
        0x0B, SHIFT, // H
        0x0C, SHIFT, // I
        0x0D, SHIFT, // J
        0x0E, SHIFT, // K
        0x0F, SHIFT, // L
        0x33, SHIFT, // M
        0x11, SHIFT, // N
        0x12, SHIFT, // O
        0x13, SHIFT, // P
        0x04, SHIFT, // Q
        0x15, SHIFT, // R
        0x16, SHIFT, // S
        0x17, SHIFT, // T
        0x18, SHIFT, // U
        0x19, SHIFT, // V
        0x1D, SHIFT, // W
        0x1B, SHIFT, // X
        0x1C, SHIFT, // Y
        0x1A, SHIFT, // Z
        0x22, SHIFT|ALT, // [ (MacFix)
        0x37, SHIFT|ALT, // bslash (MacFix)
        0x2d, SHIFT|ALT,   // ]
        0x00, NOMOD, //  0x5E
        0x2E, SHIFT, // _ (MacFix)
        0x00, NOMOD, //  0x60
        0x14, NOMOD, // a
        0x05, NOMOD, // b
        0x06, NOMOD, // c
        0x07, NOMOD, // d
        0x08, NOMOD, // e
        0x09, NOMOD, // f
        0x0A, NOMOD, // g
        0x0B, NOMOD, // h
        0x0C, NOMOD, // i
        0x0D, NOMOD, // j
        0x0E, NOMOD, // k
        0x0F, NOMOD, // l
        0x33, NOMOD, // m
        0x11, NOMOD, // n
        0x12, NOMOD, // o
        0x13, NOMOD, // p
        0x04, NOMOD, // q
        0x15, NOMOD, // r
        0x16, NOMOD, // s
        0x17, NOMOD, // t
        0x18, NOMOD, // u
        0x19, NOMOD, // v
        0x1D, NOMOD, // w
        0x1B, NOMOD, // x
        0x1C, NOMOD, // y
        0x1A, NOMOD, // z
        0x22, ALT,   // { (MacFix)
        0x0F, SHIFT|ALT, // |
        0x2d, ALT,   // } (MacFix)
        0x00, NOMOD, //  0x7E
        0x00, NOMOD, //  0x7F
};

#endif