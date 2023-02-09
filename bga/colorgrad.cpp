#include "crt.h"
#include "colorgrad.h"
#include "draw.h"

using namespace draw;

static surface pallette_bitmap;
color pallette[256];
short color_indecies[34], default_color;

void set_color(int start, int index) {
	memcpy(pallette + start, pallette_bitmap.ptr(0, index), 12 * sizeof(color));
}

void clear_indecies() {
	for(auto& i : color_indecies)
		i = -1;
	default_color = -1;
}

void set_color(const char* id) {
	auto p = bsdata<colorgrad>::find(id);
	if(!p)
		return;
	clear_indecies();
	auto index = 0;
	for(auto i : p->indecies)
		color_indecies[index++] = i;
}

void colorgrad::initialize() {
	pallette_bitmap.read("art/bmp/MPALETTE.bmp");
}