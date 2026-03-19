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


#ifdef MACHINE_RT68F

/* Custom registers */
#define LED      *(volatile UWORD*)0x4a0000

/* Video registers */
#define VGA_PLTE (void *)0x420000 // VGA Palette
#define VGA_CTRL *(volatile UWORD*)0x430000 // VGA Control

/* Serial registers */
/*
   TODO: there might be a better way to define 
         registers based on a start address
*/
#define UART_RBR *(volatile UWORD*)(0x410000) // Receive Buffer Register(RBR) / Transmitter Holding Register(THR) / Divisor Latch (LSB)
#define UART_IER *(volatile UWORD*)(0x410002) // Interrupt enable register / Divisor Latch (MSB)
#define UART_IIR *(volatile UWORD*)(0x410004) // Interrupt Identification Register
#define UART_LCR *(volatile UWORD*)(0x410006) // Line control register
#define UART_MCR *(volatile UWORD*)(0x410008) // MODEM control register
#define UART_LSR *(volatile UWORD*)(0x41000a) // Line status register
#define UART_MSR *(volatile UWORD*)(0x41000c) // MODEM status register

/* Screen Mode Bits 1-0) */
#define MODE_640X400_4COL       0x00  // 0 -> 640x400 4 colors
#define MODE_640X200_16COL      0x01  // 1 -> 640x200 16 colors
#define MODE_320X200_256CO      0x02  // 2 -> 320x200 256 colors
#define MODE_640X400_2COL       0x03  // 3 -> 640x400 2 colors

/* Screen Feature Bit Flags */
#define OVERSCAN_ON             (1 << 2) // Bit 2: 0x04
#define VBLANK_INT_ENABLE       (1 << 3) // Bit 3: 0x08
#define VBLANK_ACK              (1 << 6) // Bit 6: 0x40


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
    VEC_LEVEL3 = rt68f_vbl;
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

void rt68f_rs232_init(void) {
    // Serial is already configured by the bootloader
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

/* INT2 C handler. Called by assembler rt68f_int_timer() */
void rt68f_int_timer_c(void)
{
    rt68f_call_5ms();
    LED = hz_200;
}

#endif /* MACHINE_RT68F */
