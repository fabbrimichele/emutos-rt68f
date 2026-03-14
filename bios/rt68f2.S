/******************************************************************************/
/* memconf                                                                    */
/* Copied from amiga_memconf                                                  */
/******************************************************************************/

        .globl  rt68f_memconf
rt68f_memconf:
        lea     0,a0                    // Start looking at location 0.
        lea     0x40000,a1              // Don't look past 256KB
        
        // TODO: there is no need to look for memory, just hardcoded it to 256KB
        // TODO: I need to understand exactly what this code does, it should just
        //       set the ram to 256 and return.


checkmem:
        // On entry, A0 is the lower bound of the area to check, and A1 is
        // the high bound.  Memory is checked in 4K blocks.

        moveq   #0,d1
        move.l  d1,(a0)                 // Write a zero to the first location.
        move.l  a0,a2                   // Save the first location.
        move.l  #0xf2d4b698,d0          // Use this as a signature value.

        // Main loop:  We enter here to check each 4K block.

checkmore:
        lea     0x1000(a0),a0           // Increment current location by 4K.
        cmp.l   a0,a1                   // See if upper bound reached.
        jls     exitcheck               // If so, exit from the loop.
        move.l  d0,(a0)                 // Write the signature into memory.

        // Longword 0 of the block being checked was initially cleared to
        // zero.  If it is now no longer zero, we have "wrapped around",
        // i.e. due to incomplete address decoding, we have written the
        // signature value at the beginning of the block.  When this
        // occurs, we have reached the end of memory, even though the
        // signature value would read back correctly.

        tst.l   (a2)                    // Check location 0.
        jne     exitcheck               // Exit if signature appears there.
        cmp.l   (a0),d0                 // See if signature can be read back.
        jeq     checkmore               // If successful, go check more memory.

exitcheck:
        move.l  a0,d5                   // this is phystop - the end of RAM
        moveq   #0,d6                   // fake memctrl
        jmp     (a6)                    // Return
