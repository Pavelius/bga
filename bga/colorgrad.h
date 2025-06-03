#include "color.h"
#include "adat.h"

#pragma once

struct colorgrad {
	const char*			id;
	adat<unsigned char> indecies;
	static void			initialize();
};

extern color pallette[256];
extern short color_indecies[34]; // default_color;

void clear_indecies();
void set_color(color* pallette, int start, int index, int count = 12);
void set_color(const char* id);
