#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "a2f.h"

char quit = 0;

void cls_full() // clears both pages and sets 0,0
{
	video_page_select(video_page_r,0); // keep visible page while clearing page 0
	video_cls();
	video_page_select(0,1); // clear page 1 while viewing (clear) page 0
	video_cls();
	video_page_select(0,0); // switch to page 0 for read and write
}

void video_test_text()
{
	uint8 c = 0;
	char alt = 0;

	video_cls();
	text_xy( 1, 1); text_outs("VIDEO: TEXT PAGE 1 (F FOR PAGE 2)");
	text_xy( 4,12); text_outs("CHARACTER SET: (C TO SWITCH)");
	text_xy( 2, 3); text_outs("-- WINDOW --");
	text_xy( 2, 9); text_outs("------------");

	video_cls_page(CLS_LOW1,' '^0x80); // clear second page
	video_page_select(0,1); // write to second page
	text_xy( 1, 1); text_outs("VIDEO: TEXT PAGE 2 (F FOR PAGE 1)");
	draw_box(2,4,7,7,'+'^0x80);
	draw_hline(4,7,3,'-'^0x80);
	draw_vline(5,6,3,'|'^0x80);
	draw_fillbox(12,5,6,5,draw_getpixel(4,1));

	video_page_select(0,0); // write/view first page
	do
	{
		draw_pixel(4+(c%32),14+(c/32),c);
		++c;
	} while (c!=0);

	text_window(2,4,14,9);
	while(1)
	{
		if (kb_new())
		{
			c = kb_get();
			if      (c == KB_ESC) return;
			else if (c == 'F' || c == 'f') video_page_flip();
			else if (c == 'C' || c == 'c')
			{
				alt ^= 1;
				text_charset(alt);
			}
			c = 0;
		}
		else if (video_page_w == 0)
		{
			if      (c == 0x00) text_outs("LOW RAM");
			else if (c == 0x80) text_outs(" CHECKSUM...");
			delay(1);
			++c;
		}
	}
}

void video_test_low()
{
	char mixed = 1;
	char flip = 0;
	uint8 c;

redraw:
	cls_full();
	if (!mixed) video_mode_low();
	else        video_mode_low_mixed();
	cls_full();

	text_outs(
		"VIDEO: LOW RESOLUTION MIXED PAGE 1\n"
		"       F FOR PAGE 2\n"
		"       M FOR NON_MIXED"
	);
	draw_box(1,1,18,3,COL_WHITE);
	for (c=0; c<16; ++c) draw_pixel(2+c,2,c);
	draw_box(1,3,18,6,COL_WHITE);
	for (c=0; c<16; ++c) draw_pixel(2+c,4,draw_getpixel(2+c,2)); // test getpixel for even->even
	for (c=0; c<16; ++c) draw_pixel(2+c,5,draw_getpixel(2+c,2)); // even->odd
	for (c=0; c<16; ++c) draw_pixel(2+c,6,draw_getpixel(2+c,5)); // odd->even
	for (c=0; c<16; ++c) draw_pixel(2+c,7,draw_getpixel(2+c,5)); // odd->odd
	draw_fillbox(2,11,6,7,COL_GREEN_DARK);

	video_page_select(0,1);
	draw_fillbox(1,1,34,34,COL_GREEN_LIGHT);
	// TODO leyendecker blit?
	// TODO fine blit? masked blit?

	video_page_select(flip,flip);
	while(1)
	{
		if (kb_new())
		{
			c = kb_get();
			if      (c == KB_ESC) break;
			else if (c == 'F' || c == 'f') { video_page_flip(); flip ^= 1; }
			else if (c == 'M' || c == 'm') { mixed = !mixed; goto redraw; }
			c = 0;
		}
		else if (video_page_w != 0)
		{
			if      (c == 0x00) text_outs("LOW RAM");
			else if (c == 0x80) text_outs(" CHECKSUM...");
			delay(1);
			++c;
		}
	}
}


void keyboard_test()
{
	unsigned int count = 0;
	video_cls();
	text_xy(1,5);
	text_outs(
		"  DATA $C000:    ' '\n"
		"  ANY  $C010:\n"
		"       COUNT:");
	while(1)
	{
		uint8 d = kb_data(); // bit 7 = pending, bit 0-6 = key ($C000 read)
		char  a = kb_any(); // clears pending flag, bit 1 = held key ($C010 read)
		if (d & 0x80)
		{
			if ((d & 0x7F) == KB_ESC) break;
			++count;
		}
		text_xy(15,5); text_printf("%02X",d); draw_pixel(19,5,d ^ 0x80);
		text_xy(15,6); text_printf("%02X",a);
		text_xy(15,7); text_printf("%d",count);
		delay(30); // 30ms delay to reduce flicker
	}
}

void paddle_test()
{
	char j9[10];
	uint8 jx,jy;

	video_cls();
	text_xy(1,5);
	text_outs(
		"  BUTTONS:\n"
		"       X0:\n"
		"       Y0:\n"
		"       X1:\n"
		"       Y1:");
	while(!kb_new() || kb_get() != KB_ESC)
	{
		paddle01_poll();

		text_window(1,1,39,23);
		text_xy(12,5); text_printf("%d %d %d",
			(paddle0_b & PADDLE_B0) ? 1 : 0,
			(paddle0_b & PADDLE_B1) ? 2 : 0,
			(paddle1_b & PADDLE_B2) ? 3 : 0);
		text_xy(12,6); text_printf("$%02X %3d",paddle0_x,paddle0_x);
		text_xy(12,7); text_printf("$%02X %3d",paddle0_y,paddle0_y);
		text_xy(12,8); text_printf("$%02X %3d",paddle1_x,paddle1_x);
		text_xy(12,9); text_printf("$%02X %3d",paddle1_y,paddle1_y);

		strcpy(j9,"    X    ");
		if (paddle0_x < 128 && paddle0_y < 128)
		{
			jx = jy = 1;
			if (paddle0_x <  PADDLE_LOW ) jx -= 1;
			if (paddle0_x >= PADDLE_HIGH) jx += 1;
			if (paddle0_y <  PADDLE_LOW ) jy -= 1;
			if (paddle0_y >= PADDLE_HIGH) jy += 1;
			j9[4] = 'O';
			j9[jx+(jy*3)] = '1' ^ 0x80;
		}
		text_window(14,12,17,15); text_xy(14,12); text_outs(j9);

		strcpy(j9,"    X    ");
		if (paddle1_x < 128 && paddle1_y < 128)
		{
			jx = jy = 1;
			if (paddle1_x <  PADDLE_LOW ) jx -= 1;
			if (paddle1_x >= PADDLE_HIGH) jx += 1;
			if (paddle1_y <  PADDLE_LOW ) jy -= 1;
			if (paddle1_y >= PADDLE_HIGH) jy += 1;
			j9[4] = 'O';
			j9[jx+(jy*3)] = '2' ^ 0x80;
		}
		text_window(19,12,22,15); text_xy(19,12); text_outs(j9);
	}
}

void system_info()
{
	const char* t = "UNKNOWN";
	video_cls();
	text_xy(0,2);
	text_outs("  DETECTED SYSTEM:\n    ");
	switch(system_type)
	{
	case SYSTEM_APPLE2:      t = "APPLE 2"; break;
	case SYSTEM_APPLE2_PLUS: t = "APPLE 2 PLUS"; break;
	case SYSTEM_APPLE2E:     t = "APPLE 2E"; break;
	case SYSTEM_APPLE2C:     t = "APPLE 2C"; break;
	default:                 break;
	}
	text_outs(t);
	while (kb_get() != KB_ESC);
}

void unimplemented()
{
	video_mode_text();
	video_cls();
	text_xy(1,10);
	text_outs(" * NOT YET IMPLEMENTED\n   PRESS ESCAPE");
	// TODO beep
	while (kb_get() != KB_ESC);
}

void main_menu()
{
	cls_full();
	video_mode_text();
	video_cls();
	text_window(1,1,39,23); // 1 space border
	text_xy(1,1);
	text_outs(
		"  APPLE2FLAT DEMO\n"
		"\n"
		"ESC - RETURN TO MENU\n"
		"  1 - VIDEO: TEXT\n"
		"  2 - VIDEO: LORES\n"
		"  3 - VIDEO: HIRES COLOUR *\n"
		"  4 - VIDEO: HIRES MONO *\n"
		"  5 - VIDEO: DOUBLE TEXT *\n"
		"  5 - VIDEO: DOUBLE LORES *\n"
		"  6 - VIDEO: DOUBLE HIRES COLOUR *\n"
		"  7 - VIDEO: DOUBLE HIRES MONO *\n"
		"  A - ANIMATION *\n"
		"  K - KEYBOARD\n"
		"  P - PADDLES\n"
		"  D - DISK *\n"
		"  S - SOUND *\n"
		"  T - TEXT INPUT *\n"
		"  I - SYSTEM INFO\n"
		"* = NOT YET READY\n"
		"\n"
		" HTTPS://GITHUB.COM/\n"
		"  BBBRADSMITH/APPLE2FLAT"
	);

	switch(kb_get())
	{
	case KB_ESC: quit = 1; return; // ESCAPE (TODO: keycode enums)

	case '1': video_test_text(); break;
	case '2': video_test_low(); break;
	case 'K': case 'k': keyboard_test(); break;
	case 'P': case 'p': paddle_test(); break;
	case 'I': case 'i': system_info(); break;

	// unimplemented
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case 'A': case 'a':
	case 'B': case 'b':
	case 'D': case 'd':
	case 'S': case 's':
		unimplemented(); break;

	default: break; // TODO beep
	}
}

int main()
{
	while(!quit) main_menu();
	video_mode_text();
	video_cls();
	text_window(0,0,40,24);
	text_xy(0,21);
	text_outs("\n\nEXIT TO MONITOR:");
	return 0x1234;
}
