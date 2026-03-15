//#ifdef MACHINE_RT68F

        // Timer
        .equ tmra_ctrl, 0x00450000      // Control Register
        .equ tmra_prs,  0x00450002      // Prescaler A
        .equ tmra_cnt,  0x00450004      // Counter A

        // VGA
        .equ vga_ctrl,  0x00430000      // VGA Control
        

/******************************************************************************/
/* Interrupt 2: Keyboard and 200 Hz timer                                     */
/******************************************************************************/
        .globl  _rt68f_init_keyboard_interrupt
_rt68f_init_keyboard_interrupt:
        move.l  #rt68f_int_timer,0x68   // 0x68 is autovector interrupt level 2
        move.w  #79,tmra_prs            // Set prescaler timer A
        move.w  #999,tmra_cnt         // Set counter timer A -> 16MHz / (80 * 1000) = 200Hz
        or.w    #0x0002,tmra_ctrl       // Enable Timer A interrupt (bit 1 high)        
        rts

/******************************************************************************/
/* Interrupt 2: Timer A interrupt handler (for 200 Hz system timer)           */
/******************************************************************************/
// TODO: this breaks linef...
rt68f_int_timer:
        //move.w  #0x2700,sr              // Forbid higher level interrupts
        or.w    #0x0040,tmra_ctrl       // Acknowledge interrupt (write high to bit 6)

        // Call vector_5ms. The return address is already on the stack.
        // As it will return with RTE, we must setup a proper stack frame
        // Assume that the CPU is always a 68000 and _longframe is false
        //move.w  sr,-(sp)                // Push SR        
        
        //.extern _vector_5ms
        //move.l  _vector_5ms,a0
        //jmp     (a0)                    // Call the 200 Hz system callback   
        rte


/******************************************************************************/
/* VBL interrupt                                                              */
/******************************************************************************/
        .globl  _rt68f_vbl
_rt68f_vbl:
        or.w    #0x0040,vga_ctrl        // Ack interrupt (write high to bit 6)
        jbsr    _int_vbl                // Call the standard VBL handler
        rte


//#endif /* MACHINE_RT68F */        