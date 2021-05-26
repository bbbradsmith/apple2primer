#ifndef A2F_H
#define A2F_H

#include <stdint.h>
#include <stdarg.h>

typedef uint16_t uint16;
typedef int16_t  sint16;
typedef uint8_t  uint8;
typedef int8_t   sint8;

//
// System
//

// system type detected at startup
#define SYSTEM_UNKNOWN      0
#define SYSTEM_APPLE2       1
#define SYSTEM_APPLE2_PLUS  2
#define SYSTEM_APPLE2E      3
#define SYSTEM_APPLE2C      4
extern uint8 system_type;

// exits to monitor with program counter display
#define FATAL() { asm ("BRK"); }

// ASSERT that acts like a conditional FATAL with a custom message
// Define NDEBUG to remove asserts for a release build.
#ifdef NDEBUG
#define ASSERT(condition_,message_) {}
#else
extern void prepare_assert(const char* message);
#define ASSERT(condition_,message_) { if(!(condition_)) { prepare_assert((message_)); FATAL(); } }
#endif

//
// Keyboard
//

// some useful key codes

#define KB_ESC    0x1B
#define KB_TAB    0x09
#define KB_DELETE 0x7F
#define KB_RETURN 0x0D
#define KB_SPACE  0x20

#define KB_LEFT   0x08
#define KB_RIGHT  0x15
#define KB_UP     0x0B
#define KB_DOWN   0x0A

// Other keycodes can be identified by their ASCII char, e.g. 'A' for A key.
// Control, Shift and Capslock are not keypresses, but modify the result of other keys.
// The modification depends on the key, but example for letters:
//   K = 0x4B 'K'      (shift on)
//   K = 0x6B 'k'      (shift off)
//   K = 0x0B up arrow (control on)

char kb_new(); // 1 if a new key has been pressed (does not cancel pending)
char kb_get(); // wait for kb_new (if not already pending) and return keycode
char kb_last(); // last pressed keycode (doesn't matter if kb_new was cancelled)
uint8 kb_data(); // direct read from $C000 (bit 7 = pending new, 6-0 = keycode)
char kb_any(); // reads $C010, cancels any pending kb_new, returns 1 if any keys are currently held
// NOTE: kb_any always returns 0 on Apple II models prior to Apple IIe/IIc

// extern uint8 kb_field_cursor // TODO tile to use for field cursor
// extern uint8 kb_field_cursor_blink_rate // TODO make cursor blink after x<<8 number of polls? 0 for never
void kb_field(char* field, uint8 len); // TODO display cursor and take input with esc/delete/enter/left-right special cases, len should be 1 more than field width to allow terminal 0
// plan:
//  takes input until ESC,Tab,Enter,Up or Down is pressed
//  returns the last key pressed (allowing repeat until Enter, or other responses)
//  otherwise it will start/resume with the cursor at the 0 point in the buffer (or len-1 if it's at len)
//  should flip cursor every ??? iterations of waiting for key, maybe that's a uint16

//
// Paddle
//

// paddle_buttons mask
#define PADDLE_B0            0x01
#define PADDLE_B1            0x02
#define PADDLE_B2            0x04

// paddle axis results are approximately 0-70, centred around 32
// 255 indicates a timeout, no paddle connected
#define PADDLE_CENTER        32

// paddle poll result
extern uint8 paddle_buttons;
extern uint8 paddle0_x;
extern uint8 paddle0_y;
extern uint8 paddle1_x;
extern uint8 paddle1_y;

extern uint8 paddle_buttons_poll(); // update only paddle_buttons (and return)
extern void paddle0_poll(); // update paddle0 and buttons
extern void paddle01_poll(); // update both paddles and buttons

//
// Floppy disk
//

extern uint8_t disk_read(void* dest, uint16 sector, uint8 count);
// extern uint8 disk_write(void* src, uint16 sector, uint8 count);
extern uint8_t disk_error; // last disk error
extern uint8_t disk_volume; // last disk volume

#define DISK_ERROR_FIND      0x01
#define DISK_ERROR_DATA      0x02
#define DISK_ERROR_PARTIAL   0x04
// FIND = could not find sector on disk
// DATA = sector address found, data field missing
// PARTIAL = sector read but only partially correct (checksum fail)

//
// Tape
//

// TODO monitor read/write interface?

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

// TODO vysnc ; wait for next video frame (needs separate IIe/IIc implementation), assembly version: call system_detect first, otherwise uses fallback (or if not IIe/IIc)
// TODO extern uint16 vsync_fallback ; for Apple II/II+ use a fixed timer instead of vsync, 12.5ms by default

// video modes
extern void video_mode_text();

extern void video_cls();
extern void video_cls_page(uint8 page, uint8 fill);
// TODO video_page
// TODO video_page_flip
// TODO video_page_copy
extern void text_out(char c); // output one character at the current position
extern void text_outs(const char* s); // output a null-terminated string
extern void text_printf(const char* format, ...);
extern void text_vprintf(const char* format, va_list ap);
extern void text_scroll(sint8 lines); // positive: shift text up, clear bottom, negative: shift text down, clear top
// TODO text_charset
extern void text_xy(uint8 x, uint8 y); // set text output location (faster to set video_text_x/y directly, though)
extern void text_window(uint8 x0, uint8 y0, uint8 x1, uint8 y1); // confine text to x0<=x<x1, y0<=y<y1
extern void draw_pixel(uint16 x, uint8 y, uint8 c);
extern uint8 draw_getpixel(uint16 x, uint8 y, uint8 c);
//extern void draw_hline(uint16 x0, uint16 x1, uint8 y, uint8 c);
//extern void draw_vline(uint16 x, uint8 y0, uint8 y1, uint8 c);
//extern void draw_box(uint16 x0, uint8 y0, uint16 x1, uint8 y1, uint8 c);
//extern void draw_fillbox(uint16 x0, uint8 y0, uint16 x1, uint8 y1, uint8 c);
//extern void draw_line(uint16x, uint8 y,
//blit_tile
//blit_coarse
//blit_fine
//blit_mask

#endif
