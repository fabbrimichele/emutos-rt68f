/*
 * spi_rt68f.c - SPI interface for rt68f SD card driver
 */

#include "emutos.h"
#include "spi.h"


// Byte access -> odd addresses
#define RT68F_SD_CTRL *(volatile UBYTE*)(0x4c0001)
#define RT68F_SD_DATA *(volatile UBYTE*)(0x4c0003)

#define RT68F_TX_READY (1 << 6)


/*

It's extremesely slow, it might depend on:
- debug enabled
- SD card is not switched to higher clock 
  CHECK if that's true and how to switch
  should it be done in the spi_initialise method?
  I believe I have to implement spi_clock_sd to switch to fast clock

*/


void spi_clock_ident(void)
{
    KDEBUG(("rt68f: spi_clock_ident\n"));
    // TODO: this is HW dependent and should set the SD clock to 400 KHz
}

void spi_clock_mmc(void)
{
    KDEBUG(("rt68f: spi_clock_mmc\n"));
    // TODO
}

void spi_clock_sd(void)
{
    KDEBUG(("rt68f: spi_clock_sd\n"));
    // TODO: this is HW dependent and should switch the SD card clock 
    //       from 400KHz to the maximum BUS speed or to 25 MHz not sure yet.
}

void spi_cs_assert(void)
{
    RT68F_SD_CTRL = 0x00; // Set CS low (asserted)
    spi_send_byte(0xff);  // dummy byte to force a write to the register (other drivers do this, not sure it is required)
}

void spi_cs_unassert(void)
{   
    RT68F_SD_CTRL = 0xff; // Set CS high (unasserted)
    spi_send_byte(0xff);  // dummy byte to force a write to the register (other drivers do this, not sure it is required)
}

void spi_initialise(void)
{
    KDEBUG(("rt68f: spi_initialise\n"));
    // I think there is no need to initialize anything.
    // The initialization is done in `sd_check(...)`
}

UBYTE spi_recv_byte(void)
{
    spi_send_byte(0xff);        // dummy write
    UBYTE data = RT68F_SD_DATA;    
    return data;
}

void spi_send_byte(UBYTE input)
{    
    while(!(RT68F_SD_CTRL & RT68F_TX_READY)) {} // ready when RT68F_TX_READY == 1
    RT68F_SD_DATA = input;
    // TODO: check if I need to wait for the response
}
