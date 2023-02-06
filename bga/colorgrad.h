#include "crt.h"
#include "color.h"

#pragma once

struct colorgrad {
	const char*			id;
	adat<unsigned char> indecies;
};

extern color pallette[256];
extern short color_indecies[34], default_color;

void clear_indecies();
void set_color(int start, int index);
void set_color(const char* id);
