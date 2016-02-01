#include <libnus.h>
#include <libfb.h>
#include <regsinternal.h>
#include "font8x8_basic.h"

#include <stdlib.h>
#include <stdint.h>

//Temporary frame buffer library

uint32_t __attribute__ ((aligned(16))) fb[240][320];
uint32_t (*fbnc)[320];

VI_regs_t vi_state = {
	0x00003243, // status
	0x00100000, // origin
	0x00000140, // width
	0x00000002, // intr
	0x00000000, // current
	0x03E52239, // burst
	0x0000020D, // v_sync
	0x00000C15, // h_sync
	0x0C150C15, // leap
	0x006C02EC, // h_start
	0x002501FF, // v_start
	0x000E0204, // v_burst
	0x00000200, // x_scale
	0x00000400, // y_scale
};

void fb_init(){
	fbnc = (uint32_t (*)[320]) ((intptr_t)fb | 0xa0000000);
	vi_state.framebuffer = (void*) fbnc;
	nus_vi_flush_state(&vi_state);
}

void fb_print_char(int x, int y, char c){
	for(int row = 0; row < 8; row++){
		if(y + row >= 240) break;
		int part = font8x8_basic[(int)c][row];
		for(int column = 0; column < 8; column++){
			if(x + column >= 320) break;
			if(part & (1 << (column))) fbnc[y+row][x+column] = ~0;
		}
	}
}

void fb_print_string(int x, int y, char *str){
	int pos = 0;
	while(str[pos]){
		fb_print_char(x + pos * 8, y, (char) str[pos]);
		pos++;
	}	
}

void fb_print_int(int x, int y, int value, int base){
	char str[16];
	itoa(value, str, base);
	fb_print_string(x,y,str);
}

void fb_blue_background(){
	for(int y = 0; y < 240; y++){
		for(int x = 0; x < 320; x++){
			uint16_t color = y;
			fbnc[y][x] = (0 << 24) | ((color / 2) << 16) | (color << 8) | (0xff << 0);
		}
	}

}

void fb_red_background(){
	for(int y = 0; y < 240; y++){
		for(int x = 0; x < 320; x++){
			uint16_t color = y;
			fbnc[y][x] = (color << 24) | ((color / 2) << 16) | (0 << 8) | (0xff << 0);
		}
	}
}
