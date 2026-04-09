/*
 * rt68f.c - rt68f specific functions
 *
 * Copyright (C) 2013-2025 The EmuTOS development team
 *
 * Authors:
 *  MF   Michele Fabbri
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "emutos.h"
#include "rt68f.h"
#include "vectors.h"
#include "tosvars.h"
#include "ikbd.h"
#include "serport.h"

#ifdef MACHINE_RT68F

/* Custom registers */
#define LED      *(volatile UWORD*)0x4a0000

/* Video registers */
#define VGA_PLTE (void *)0x420000 // VGA Palette
#define VGA_CTRL *(volatile UWORD*)0x430000 // VGA Control

/* Serial registers */
#define UART_RBR *(volatile UWORD*)(0x410000) // Receive Buffer Register(RBR) / Transmitter Holding Register(THR) / Divisor Latch (LSB)
#define UART_IER *(volatile UWORD*)(0x410002) // Interrupt enable register / Divisor Latch (MSB)
#define UART_IIR *(volatile UWORD*)(0x410004) // Interrupt Identification Register
#define UART_LCR *(volatile UWORD*)(0x410006) // Line control register
#define UART_MCR *(volatile UWORD*)(0x410008) // MODEM control register
#define UART_LSR *(volatile UWORD*)(0x41000a) // Line status register
#define UART_MSR *(volatile UWORD*)(0x41000c) // MODEM status register

/* Keyboard & Mouse registers */
#define PS2A_CTRL *(volatile UWORD*)(0x470000) // Control Register
#define PS2A_DATA *(volatile UWORD*)(0x470002) // Data Register
#define PS2B_CTRL *(volatile UWORD*)(0x480000) // Control Register
#define PS2B_DATA *(volatile UWORD*)(0x480002) // Data Register

/* Screen Mode Bits 1-0) */
#define MODE_640X400_4COL       0x00  // 0 -> 640x400 4 colors
#define MODE_640X200_16COL      0x01  // 1 -> 640x200 16 colors
#define MODE_320X200_256CO      0x02  // 2 -> 320x200 256 colors
#define MODE_640X400_2COL       0x03  // 3 -> 640x400 2 colors

/* Screen Feature Bit Flags */
#define OVERSCAN_ON             (1 << 2) // Bit 2: 0x04
#define VBLANK_INT_ENABLE       (1 << 3) // Bit 3: 0x08
#define VBLANK_ACK              (1 << 6) // Bit 6: 0x40

/* PS/2 Feature Bit Flags */
#define PS2_INT_ENABLE          (1 << 1) // Bit 1: 0x02

/* PS/2 & IDKB constants */
#define PS2_BREAK_CODE          0xf0
#define PS2_EXT_CODE            0xe0
#define PS2_EXT_CODE1           0xe1
#define IDKB_BREAK              0x80
#define IDKB_CAPSLOCK           0x3a

static UBYTE ps2_mouse_buf[3];
static UBYTE ps2_mouse_buf_index;
static BOOL  ps2_keyb_is_break;
static BOOL  ps2_keyb_is_ext;


/* Initialize Native Features
*  do it as soon as possible so that kprintf can make use of them
*/
void rt68f_init(void) 
{
    LED = 0x0;   // Debug
}

/******************************************************************************/
/* Screen                                                                     */
/******************************************************************************/

UWORD* pword_vga_palette = (UWORD *)VGA_PLTE;
const UBYTE *rt68f_screenbase;

/* 
 * Initialize graphic palette and video mode 
 */
void rt68f_screen_init(void)
{
    // Set palette colors:
    pword_vga_palette[0] = 0x0FFF; // color 0 xRGB (white)
    pword_vga_palette[1] = 0x0000; // color 1 xRGB (black)

    /* Set VBL interrupt routine */
    VEC_LEVEL3 = rt68f_vbl_int;
    VGA_CTRL = MODE_640X400_2COL | VBLANK_INT_ENABLE;
}

ULONG rt68f_vram_size(void)
{
    return 64000UL;
}

/*
 * returns the palette (number of colour choices) for the current hardware
 */
WORD rt68f_get_palette(void)
{
    return 2;
}

void rt68f_get_current_mode_info(UWORD *planes, UWORD *hz_rez, UWORD *vt_rez)
{
    *planes = 1;
    *hz_rez = 640;
    *vt_rez = 400;
}

void rt68f_setphys(const UBYTE *addr)
{
    rt68f_screenbase = addr;
}

const UBYTE *rt68f_physbase(void)
{
    return rt68f_screenbase;
}


/******************************************************************************/
/* RS232                                                                      */
/******************************************************************************/

void rt68f_rs232_init(void) 
{
    // Settings inhereted from boot loader

    VEC_LEVEL4 = rt68f_rs232_int; // Set interrupt handler
    UART_IER = 0x01;              // Enable interrupt on receive holding register
    // TODO: define an constant for  0x01

    LED = 1;
}

void rt68f_rs232_int_c(void) 
{
    if (UART_IIR != 4) // Check received rata ready and clean interrupt
        return;        // If not Received Data Ready, return

    char in_b = UART_RBR;
    push_serial_iorec(in_b); // Read and push serial input byte    
}


BOOL rt68f_rs232_can_write(void)
{
    // Check if space is available in the FIFO
    return UART_LSR & 0x20; // Bit 5 - TODO: use a constant
}

void rt68f_rs232_write_byte(UBYTE b)
{
    while (!rt68f_rs232_can_write())
    {
        // Wait
    }
    
    // Send the byte
    UART_RBR = (UWORD)b;
}

void kprintf_outc_rt68f_rs232(int c)
{
    // Raw terminals usually require CRLF 
    if ( c == '\n')
        rt68f_rs232_write_byte('\r');

    rt68f_rs232_write_byte((char)c);
}


/******************************************************************************/
/* Timer                                                                      */
/******************************************************************************/
void rt68f_init_system_timer(void)
{
}

/* INT2 C handler. Called by assembler rt68f_timer_int() */
void rt68f_timer_int_c(void)
{
    rt68f_call_5ms();
    //LED = hz_200;
}

/******************************************************************************/
/* IKBD                                                                       */
/* Documentation: https://www.kernel.org/doc/Documentation/input/atarikbd.txt */
/******************************************************************************/
void rt68f_kbd_mouse_init(void)
{
    KDEBUG(("rt68f_kbd_mouse_init\n"));

    // Reset keyboard
    PS2A_DATA = 0xff;

    // Enable mouse stream
    PS2B_DATA = 0xf4;

    // Reset mouse buffer index
    ps2_mouse_buf_index = 0;

    // Reset key 
    ps2_keyb_is_break = FALSE;

    // Set interrupt handlers
    VEC_LEVEL5 = rt68f_kbd_int;
    VEC_LEVEL6 = rt68f_mouse_int;

    // Enable PS/2 interrupts
    PS2A_CTRL = PS2_INT_ENABLE;
    PS2B_CTRL = PS2_INT_ENABLE;
}

/* 
    PS/2 to Atari mapping 
    PS/2: https://wiki.osdev.org/PS/2_Keyboard
    Atari: https://www.kernel.org/doc/Documentation/input/atarikbd.txt
    Emulated keys: https://www.hatari-emu.org/doc/manual.html

    Map:
    - F11 to HELP
    - F12 to UNDO
*/
const UBYTE ps2_to_idkb_map[256] = {
    // 0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
    0x00, 0x43, 0x00, 0x3f, 0x3d, 0x3b, 0x3c, 0x61, 0x00, 0x44, 0x42, 0x40, 0x3e, 0x0f, 0x29, 0x00, // 00
    0x00, 0x38, 0x2a, 0x00, 0x1d, 0x10, 0x02, 0x00, 0x00, 0x00, 0x2c, 0x1f, 0x1e, 0x11, 0x03, 0x00, // 10
    0x00, 0x2e, 0x2d, 0x20, 0x12, 0x05, 0x04, 0x00, 0x00, 0x39, 0x2f, 0x21, 0x14, 0x13, 0x06, 0x00, // 20
    0x00, 0x31, 0x30, 0x23, 0x22, 0x15, 0x07, 0x00, 0x00, 0x00, 0x32, 0x24, 0x16, 0x08, 0x09, 0x00, // 30
    0x00, 0x33, 0x25, 0x17, 0x18, 0x0b, 0x0a, 0x00, 0x00, 0x34, 0x35, 0x26, 0x27, 0x19, 0x0c, 0x00, // 40
    0x00, 0x00, 0x28, 0x00, 0x1a, 0x0d, 0x00, 0x00, 0x3a, 0x36, 0x1c, 0x1b, 0x00, 0x2b, 0x00, 0x00, // 50
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x6d, 0x00, 0x6a, 0x67, 0x00, 0x00, 0x00, // 60
    0x70, 0x71, 0x6e, 0x6b, 0x6c, 0x68, 0x01, 0x00, 0x62, 0x4e, 0x6f, 0x4a, 0x66, 0x69, 0x61, 0x00, // 70
    0x00, 0x00, 0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 80
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 90
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // A0
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // B0
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // C0
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // D0
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // E0
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // F0
};
const UBYTE ps2_to_idkb_ext_map[256] = {
    // 0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 00
    0x00, 0x38, 0x2a, 0x00, 0x1d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 10
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 20
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 30
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00, // 40
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, // 50
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4b, 0x47, 0x00, 0x00, 0x00, // 60
    0x52, 0x53, 0x50, 0x00, 0x4d, 0x48, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x63, 0x00, 0x00, // 70
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 80
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 90
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // A0
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // B0
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // C0
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // D0
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // E0
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // F0
};

/*
    # TODO: Print Screen
    expected: E0 12 E0 7C / E0 F0 7C E0 F0 12

    There is a bug somewhere, the actual sequence 
    received is missing some codes and it's not even 
    consistent. It might be a problem in the FPGA component.
    Example of code sequence for print screen:
    - e0 12 7c e0 f0 e0 f0 12		
    - e0 12 7c e0 f0 04 f0 12
    
    # TODO: Pause Break
    It might be affected by the transmission bug
    E1 14 77 E1/F0 14 F0 77,0x00,Pause Break
*/
void rt68f_kbd_int_c(void)
{
    UWORD ps2_code = PS2A_DATA;
    KDEBUG(("rt68f_kbd_int_c ps2_code = 0x%02x\n", ps2_code));

    if (ps2_code == PS2_BREAK_CODE) 
    {
        ps2_keyb_is_break = TRUE;
        return;
    } 
    if (ps2_code == PS2_EXT_CODE) 
    {
        ps2_keyb_is_ext = TRUE;
        return;
    } 

    UBYTE idkb_code = 0;
    if (ps2_keyb_is_ext) 
    {
        idkb_code = ps2_to_idkb_ext_map[ps2_code];
        ps2_keyb_is_ext = FALSE;
    }
    else {
        idkb_code = ps2_to_idkb_map[ps2_code];    
    }

    if (idkb_code != 0) {
        if (ps2_keyb_is_break) {
            idkb_code |= IDKB_BREAK;
            ps2_keyb_is_break = FALSE;
        }
    
        KDEBUG(("rt68f_kbd_int_c idkb_code = 0x%02x\n", idkb_code));
        call_ikbdraw(idkb_code);
    }
}

static void rt68f_mouse_send_packet(SBYTE dx, SBYTE dy, BOOL btn_left, BOOL btn_right)
{
    SBYTE packet[3];
    packet[0] = 0xf8; /* IKBD mouse packet header */

    if (btn_right)
        packet[0] |= 0x01;

    if (btn_left)
        packet[0] |= 0x02;

    packet[1] = dx;
    packet[2] = dy;

    // Send mouse packet to IKBD handler
    call_mousevec(packet);

}

void rt68f_mouse_int_c(void) {
    SBYTE mouse_packet = PS2B_DATA;

    // Sync check: Byte 0 must have bit 3 set to 1
    if (ps2_mouse_buf_index == 0 && !(mouse_packet & 0x08))
        return; // Out of sync, ignore this byte

    ps2_mouse_buf[ps2_mouse_buf_index++] = mouse_packet;

    if (ps2_mouse_buf_index == 3) 
    {
        // Full packet received
        BOOL left_button = ps2_mouse_buf[0] & 0x01;
        BOOL right_button = ps2_mouse_buf[0] & 0x02;
        SBYTE dx = ps2_mouse_buf[1];
        SBYTE dy = -ps2_mouse_buf[2];

        rt68f_mouse_send_packet(dx, dy, left_button, right_button);

        // Reset index
        ps2_mouse_buf_index = 0;
    }
}

void rt68f_ikbd_writeb(UBYTE b)
{
    KDEBUG(("rt68f_ikbd_writeb 0x%02x\n", b));

    // TODO: Handle all commands
    // Note: mouse and keyboard apparently work without this.
    //       However some corner cases might not work properly.

    /* commands sent when EmuTOS start
    RESET
    rt68f_ikbd_writeb 0x80 
    rt68f_ikbd_writeb 0x01    

    SET RELATIVE MOUSE POSITION REPORTING (DEFAULT)        
    rt68f_ikbd_writeb 0x08
    
    SET MOUSE THRESHOLD        
    rt68f_ikbd_writeb 0x0b (COMMAND)
    rt68f_ikbd_writeb 0x01 (x threshold in mouse ticks (positive integers))
    rt68f_ikbd_writeb 0x01 (y threshold in mouse ticks (positive integers))

    SET Y=0 AT TOP (mouse)
    rt68f_ikbd_writeb 0x10

    SET MOUSE BUTTON ACTION
    rt68f_ikbd_writeb 0x07 (COMMAND)
    rt68f_ikbd_writeb 0x00        
    */
}

#endif /* MACHINE_RT68F */
