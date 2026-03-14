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


#ifdef MACHINE_RT68F

/* Custom registers */
#define LED      *(volatile UWORD*)0x4a0000

/* Video registers */
#define VGA_PLTE (void *)0x420000 // VGA Palette
#define VGA_CTRL *(volatile UWORD*)0x430000 // VGA Control

/* Serial registers */
// TODO


/* Screen Mode Bits 1-0) */
#define MODE_640X400_4COL       0x00  // 0 -> 640x400 4 colors
#define MODE_640X200_16COL      0x01  // 1 -> 640x200 16 colors
#define MODE_320X200_256CO      0x02  // 2 -> 320x200 256 colors
#define MODE_640X400_2COL       0x03  // 3 -> 640x400 2 colors

/* Feature Bit Flags */
#define OVERSCAN_ON             (1 << 2) // Bit 2: 0x04
#define VBLANK_INT_ENABLE       (1 << 3) // Bit 3: 0x08
#define VBLANK_ACK              (1 << 6) // Bit 6: 0x40


/* Initialize Native Features
*  do it as soon as possible so that kprintf can make use of them
*/
void rt68f_init(void) 
{
    LED = 0x1;   // Debug
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
    VGA_CTRL = MODE_640X400_2COL | OVERSCAN_ON;

    // Set palette colors:
    pword_vga_palette[0] = 0x0FFF; // color 0 xRGB (white)
    pword_vga_palette[1] = 0x0000; // color 1 xRGB (black)
    
    LED = 0x2; // Debug
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
/* RS232                                                                     */
/******************************************************************************/

void rt68f_rs232_init(void) {
    // Serial is already configured by the bootloader
    LED = 0x3; // Debug
}

#endif /* MACHINE_RT68F */
