#include "color.h"
#include "point.h"
#include "stringbuilder.h"

#pragma once

struct sprite;

namespace map {

extern unsigned char heightmap[256 * 256];
extern unsigned char statemap[256 * 256];
extern unsigned char lightmap[256 * 256];
extern unsigned short tilemap[64 * 64];

int getday(unsigned value);
int gethour(unsigned value);
int gettile(short unsigned index);

unsigned char getorientation(point s, point d);
unsigned char getstate(short unsigned index);
unsigned short getindex(point pos);
unsigned short getindex(point pos, int size);
point getfree(point position, int size);
point getposition(short unsigned index, int size);

void getpassedtime(stringbuilder& sb, unsigned value);
void read(const char* name);
void settile(short unsigned index, short unsigned tile);

color getshadow(point s);
color* getpallette();

const sprite* getminimap();
const sprite* getareasprite();

}