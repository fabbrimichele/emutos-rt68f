/*
 * spi_rt68f.c - SPI interface for rt68f SD card driver
 */

#include "emutos.h"
#include "spi.h"

// This alone doesn't work
#define ENABLE_KDEBUG

// Byte access -> odd addresses
#define RT68F_SPI_DTLW *(volatile UBYTE*)(0x4c0001) // Data LSB
#define RT68F_SPI_DTHI *(volatile UBYTE*)(0x4c0003) // Data MSB
#define RT68F_SPI_CDST *(volatile UBYTE*)(0x4c0005) // Command/Status
#define RT68F_SPI_CONF *(volatile UBYTE*)(0x4c0007) // Config

/*

  CS: Write bits:
  CDST[0]   START : Start transfer
  CDST[2]   IRQEN : Generate IRQ at end of transfer
  CDST[3]   CS    : CS (chip select): 1 asserted, 0 unsserted
  CDST[6:4] SPIAD : SPI device address
  
  CDST: Read bits
  CDST[0]   BUSY  : Currently transmitting data
  
  CONF: Write bits
  CONF[2:0] DIVIDE: SPI clock divisor,
                    000=clk/2,
                    001=clk/4,
                    010=clk/8,
                    011=clk/16
                    100=clk/32,
                    101=clk/64,
                    110=clk/128,
                    111=clk/256
  CO[4:3] LENGTH: Transfer length,
                  00= 4 bits,
                  01= 8 bits,
                  10=12 bits,
                  11=16 bits
*/


void spi_clock_ident(void)
{
    KDEBUG(("rt68f: spi_clock_ident\n"));
    RT68F_SPI_CONF = 0b01101; // 01 transfer = 8 bits, 101 clock divisor = clk/64 (16MHz/64=250KHz)
}

void spi_clock_mmc(void)
{
    KDEBUG(("rt68f: spi_clock_mmc\n"));
    // TODO
}

void spi_clock_sd(void)
{
    KDEBUG(("rt68f: spi_clock_sd\n"));
    RT68F_SPI_CONF = 0b01000; // 01 transfer = 8 bits, 101 clock divisor = clk/2 (16MHz/2=8MHz)    
}

void spi_cs_assert(void)
{
    // Write to Command Register
    // Clear the deselect bit (bit 1) to 0. 
    // This ensures that when we start, the CS stays low.
    //RT68F_SPI_CDST |= 0b00000010; // CS asserted CDST[1]=1 
    RT68F_SPI_CDST = 0b00000010;
    spi_send_byte(0xff);  // dummy byte to force a write to the register (other drivers do this, not sure it is required)

}

void spi_cs_unassert(void)
{   
    //RT68F_SPI_CDST &= 0b11111101; // CS unasserted CDST[3]=0
    RT68F_SPI_CDST = 0b00000000;
    spi_send_byte(0xff);  // dummy byte to force a write to the register (other drivers do this, not sure it is required)
}

void spi_initialise(void)
{
    KDEBUG(("rt68f: spi_initialise\n"));
    RT68F_SPI_CONF = 0b00001101; // 01 transfer = 8 bits, 101 clock divisor = clk/64 (16MHz/64=250KHz)
    RT68F_SPI_CDST = 0b00000000; // SPI address CDST[6:4]=0, interrupt dis CDST[2]=0, CS CDST[1]=0, stop CDST[0]=0
}

UBYTE spi_recv_byte(void)
{
    spi_send_byte(0xff);    // Trigger 8 clock pulses
    return RT68F_SPI_DTLW;    
}

void spi_send_byte(UBYTE input)
{    
    // 1. Wait for BUSY bit (bit 0 of RT68F_SPI_CDST) to be 0
    while(RT68F_SPI_CDST & 0x01);

    // 2. Load the data into Data Low (Address 0)
    RT68F_SPI_DTLW = input;

    // 3. Start transfer: assert Start CDST[0] = 1
    //RT68F_SPI_CDST |= 0x01;
    RT68F_SPI_CDST = 0b00000011;

    // 4. Wait for transfer to finish before returning 
    // (required to finish reading commands)
    while(RT68F_SPI_CDST & 0x01);
}
