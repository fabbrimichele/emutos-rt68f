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
// Configure interrupt
        .globl  _rt68f_init_keyboard_interrupt
_rt68f_init_keyboard_interrupt:
        move.l  #rt68f_int_timer,0x68   // 0x68 is autovector interrupt level 2
        move.w  #79,tmra_prs            // Set prescaler timer A
        move.w  #999,tmra_cnt         // Set counter timer A -> 16MHz / (80 * 1000) = 200Hz
        or.w    #0x0002,tmra_ctrl       // Enable Timer A interrupt (bit 1 high)        
        rts

        // Interrupt handler
rt68f_int_timer:
        // Clear the hardware interrupt IMMEDIATELY
        // This prevents the timer from re-triggering while we are in the handler
        or.w    #0x0040,tmra_ctrl       // Acknowledge interrupt

        // ************  BUG ************************************************
        // It gets stuck in the boot screen, likely the problem is 
        // is that I'm forbidding the wrong interrupts in the following line:
        // Could it be a calibration issue? I see a warning in the log
        // calibrate_delay()
        // Warning: loopcount_1_msec isn't calibrated.
        // calibrate_delay loopcount_1_msec=760


        move.w  #0x2700,sr              // Forbid higher level interrupts
                                        // Original interrupt mask will be restored by RTE
        movem.l d0-d1/a0-a1,-(sp)
        jbsr    _rt68f_int_1_c           // Delegate to C implementation
        movem.l (sp)+,d0-d1/a0-a1
        rte           

// Call vector_5ms (200 Hz system timer callback) from C.
// As it will return with RTE, we must setup a proper stack frame.
        .globl  _rt68f_call_5ms
_rt68f_call_5ms:
        // The return address is already on the stack.
        // Assume that the CPU is always a 68000 and _longframe is false
        move.w  sr,-(sp)                // Push SR
        .extern _vector_5ms
        move.l  _vector_5ms,a0
        jmp     (a0)                    // Call the 200 Hz system callback           


/******************************************************************************/
/* VBL interrupt handler                                                              */
/******************************************************************************/
        .globl  _rt68f_vbl
_rt68f_vbl:
        // Clear the hardware interrupt IMMEDIATELY
        // This prevents the timer from re-triggering while we are in the handler
        or.w    #0x0040,vga_ctrl        // Ackownledge interrupt
        jbsr    _int_vbl                // Call the standard VBL handler
        rte


//#endif /* MACHINE_RT68F */        