#ifndef A2F_H
#define A2F_H

#include <stdint.h>

typedef uint16_t uint16;
typedef int16_t  sint16;
typedef uint8_t  uint8;
typedef int8_t   sint8;

//
// Floppy disk
//

extern uint8_t disk_read(void* dest, uint16 sector, uint8 count);
extern uint8_t disk_error; // last disk error
extern uint8_t disk_volume; // last disk volume

#define DISK_ERROR_FIND      0x01
#define DISK_ERROR_DATA      0x02
#define DISK_ERROR_PARTIAL   0x04
// FIND = could not find sector on disk
// DATA = sector address found, data field missing
// PARTIAL = sector read but only partially correct (checksum fail)

//
// Video
//

// page definitions for video_cls_page
#define CLS_LOW0    0
#define CLS_LOW1    1
#define CLS_HIGH0   2
#define CLS_HIGH1   3

extern uint8 video_text_x;
extern uint8 video_text_y;
extern uint8 video_text_w;
extern uint8 video_text_h;
extern uint8 video_text_xr;
extern uint8 video_text_yr;
extern uint8 video_page_w;
extern uint8 video_page_r;
extern uint8 text_inverse;

// video modes
extern void video_mode_text();

extern void video_cls();
extern void video_cls_page(uint8 page, uint8 fill);
extern void text_out(char c); // output one character at the current position
extern void text_scroll(sint8 lines); // positive: shift text up, clear bottom, negative: shift text down, clear top
extern void text_window(uint8 x0, uint8 y0, uint8 x1, uint8 y1); // confine text to x0<=x<x1, y0<=y<y1
extern void draw_pixel(uint16 x, uint8 y, uint8 c);
extern uint8 draw_getpixel(uint16 x, uint8 y, uint8 c);
extern void draw_hline(uint16 x0, uint16 x1, uint8 y, uint8 c);
extern void draw_vline(uint16 x, uint8 y0, uint8 y1, uint8 c);
extern void draw_box(uint16 x0, uint8 y0, uint16 x1, uint8 y1, uint8 c);
extern void draw_fillbox(uint16 x0, uint8 y0, uint16 x1, uint8 y1, uint8 c);
//blit_tile
//blit_coarse
//blit_fine
//blit_mask

#endif
