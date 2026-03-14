#ifndef RT68F_H
#define RT68F_H
#ifdef MACHINE_RT68F

void rt68f_init(void);

void rt68f_screen_init(void);
ULONG rt68f_vram_size(void);
WORD rt68f_get_palette(void);
void rt68f_get_current_mode_info(UWORD *planes, UWORD *hz_rez, UWORD *vt_rez);
void rt68f_setphys(const UBYTE *addr);
const UBYTE *rt68f_physbase(void);
void rt68f_rs232_init(void);
BOOL rt68f_rs232_can_write(void);
void rt68f_rs232_write_byte(UBYTE);
void kprintf_outc_rt68f_rs232(int);

#endif /* MACHINE_RT68F */
#endif /* RT68_H */
