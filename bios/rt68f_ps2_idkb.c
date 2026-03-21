#include "emutos.h"

#ifdef MACHINE_RT68F

const UBYTE ps2_idkb_map[256] = {
  0,    // 0
  0,
  0,
  0x3A, // 3 CAPSLOCK !!DOES NOT WORK!!
  0,    // 4
  0,    // 5
  0x2A, // 6 LEFT SHIFT
  0x36, // 7 RIGHT SHIFT

  0x1D, // 8 CTRL
  0x1D, // 9 CTRL
  0x38, // 10 ALT
  0x38, // 11 ALT 
  0,
  0,
  0,
  0,
    
  0,    // 16
  0x47, // 17 HOME
  0,
  0,
  0,
  0x4B, // 21 LEFT ARROW
  0x4D, // 22 RIGHT ARROW
  0x48, // 23 UP ARROW
    
  0x50, // 24 DOWN ARROW
  0x52, // 25 INSERT
  0x53, // 26 DEL
  0x01, // 27 ESC
  0x0E, // 28 BACKSPACE
  0x0F, // 29 TAB   
  0x1C, // 30 RETURN
  0x39, // 31 SPACE BAR

  0x70, // 32 KEYPAD 0
  0x6D, // 33 KEYPAD 1
  0x6E, // 34 KEYPAD 2
  0x6F, // 35 KEYPAD 3
  0x6A, // 36 KEYPAD 4
  0x6B, // 37 KEYPAD 5
  0x6C, // 38 KEYPAD 6
  0x67, // 39 KEYPAD 7

  0x68, // 40 KEYPAD 8
  0x69, // 41 KEYPAD 9
  0x71, // 42 KEYPAD .
  0x72, // 43 KEYPAD ENTER
  0x4E, // 44 KEYPAD +
  0x4A, // 45 KEYPAD -
  0x66, // 46 KEYPAD *
  0x65, // 47 KEYPAD /

  0x0B, // 48 '0'
  0x02, // 49 '1'
  0x03, // 50 '2'
  0x04, // 51 '3'
  0x05, // 52 '4'
  0x06, // 53 '5'
  0x07, // 54 '6'
  0x08, // 55 '7'

  0x09, // 56 '8'
  0x0A, // 57 '9'
  0x28, // 58 '''
  0x33, // 59 ','
  0x0C, // 60 '-'
  0x34, // 61 '.'
  0x35, // 62 '/'
  0,

  0x29, // 64 '`'
  0x1E, // 65 'A'
  0x30, // 66 'B'
  0x2E, // 67 'C'
  0x20, // 68 'D'
  0x12, // 69 'E'
  0x21, // 70 'F'
  0x22, // 71 'G'

  0x23, // 72 'H'
  0x17, // 73 'I'
  0x24, // 74 'J'
  0x25, // 75 'K'
  0x26, // 76 'L'
  0x32, // 77 'M'
  0x31, // 78 'N'
  0x18, // 79 'O'

  0x19, // 80 'P'
  0x10, // 81 'Q'
  0x13, // 82 'R'
  0x1F, // 83 'S'
  0x14, // 84 'T'
  0x16, // 85 'U'
  0x2F, // 86 'V'
  0x11, // 87 'W'
    
  0x2D, // 88 'X'
  0x15, // 89 'Y'
  0x2C, // 90 'Z'
  0x27, // 91 ';'
  0x00, // 92 '#' // Not mapped
  0x1A, // 93 '['
  0x1B, // 94 ']'
  0x0D, // 95 '='
    
  0,  // 96
  0x3B, // 97 F1
  0x3C, // 98 F2
  0x3D, // 99 F3
  0x3E, // 100 F4
  0x3F, // 101 F5
  0x40, // 102 F6
  0x41, // 103 F7

  0x42, // 104 F8
  0x43, // 105 F9
  0x44, // 106 F10
  0x62, // 107 F11 -> HELP
  0x61, // 108 F12 -> UNDO
  0,
  0,
  0,

  0,  // 112
  0,
  0,
  0,
  0,
  0,
  0,
  0,

  0,  // 120
  0,
  0,
  0,
  0,
  0,
  0,
  0,

  0,  // 128
  0,
  0,
  0,
  0,
  0,
  0,
  0,

  0,    // 136
  0,    // 137
  0,    // 138
  0x2B, // 139 '\'
  0,
  0,
  0,
  0
};

#endif /* MACHINE_RT68F */