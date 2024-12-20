#include "color.h"
#include "point.h"
#include "stringbuilder.h"
#include "slice.h"
#include "variable.h"

#pragma once

enum areaf_s : unsigned char {
	StateExplored, CreatureBlock,
};

struct sprite;

struct areai {
	char			name[12];
	char			folder[4];
	slice<variable>	variables;
	static areai*	add(const char* name, const char* folder);
	static int		compare(const void* v1, const void* v2);
	static areai*	find(const char* name, const char* folder);
};
extern unsigned short current_area;

namespace map {

extern unsigned char	heightmap[256 * 256];
extern unsigned char	statemap[256 * 256];
extern unsigned char	lightmap[256 * 256];
extern unsigned short	tilemap[64 * 64];
extern char				areaname[12];

int gettile(short unsigned index);

unsigned char getorientation(point s, point d);
unsigned char getstate(short unsigned index);

unsigned short getindex(point pos);
unsigned short getindex(point pos, int size);

point getfree(point position, int size);
point getposition(short unsigned index, int size);

void read(const char* name);
void set(unsigned short index, areaf_s v);
void settile(short unsigned index, short unsigned tile);

color getshadow(point s);
color* getpallette();

const sprite* getminimap();
const sprite* getareasprite();

bool is(unsigned short index, areaf_s v);

}

void setcamera(point v);