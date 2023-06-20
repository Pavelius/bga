#include "crt.h"
#include "color.h"
#include "point.h"
#include "stringbuilder.h"

#pragma once

struct sprite;

struct areai {
	char		name[12];
	char		folder[4];
	static areai* add(const char* name, const char* folder);
	static int	compare(const void* v1, const void* v2);
	static areai* find(const char* name, const char* folder);
};
extern unsigned short current_area;

namespace map {

extern unsigned char heightmap[256 * 256];
extern unsigned char statemap[256 * 256];
extern unsigned char lightmap[256 * 256];
extern unsigned short tilemap[64 * 64];
extern char areaname[12];

int getday(unsigned value);
int gethour(unsigned value);
int gettile(short unsigned index);

unsigned char getorientation(point s, point d);
unsigned char getstate(short unsigned index);

unsigned short getindex(point pos);
unsigned short getindex(point pos, int size);

point getfree(point position, int size);
point getposition(short unsigned index, int size);

void enter(const char* area, const char* entrance);
void getpassedtime(stringbuilder& sb, unsigned value);
void read(const char* name);
void settile(short unsigned index, short unsigned tile);

color getshadow(point s);
color* getpallette();

const sprite* getminimap();
const sprite* getareasprite();

}

void setcamera(point v);