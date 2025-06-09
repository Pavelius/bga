#include "color.h"
#include "point.h"
#include "stringbuilder.h"
#include "slice.h"
#include "variable.h"

#pragma once

enum areaf_s : unsigned char {
	StateExplored, CreatureBlock,
};
enum directionn : unsigned char;

const short unsigned Blocked = 0xFFFF;

struct sprite;

extern char	area_name[12];
extern unsigned char area_zmap[256 * 256];
extern unsigned char area_light[256 * 256];
extern unsigned char area_state[256 * 256];
extern unsigned short area_tiles[64 * 64];
extern unsigned short area_cost[256 * 256];
extern unsigned short area_width, area_height, area_height_tiles;

struct areai {
	char			name[12];
	char			folder[4];
	slice<variable>	variables;
	static int compare(const void* v1, const void* v2);
	static areai* add(const char* name, const char* folder);
	static areai* find(const char* name, const char* folder);
};
extern unsigned short current_area;

unsigned char get_look(point s, point d); // Determine orientation
unsigned char get_state(short unsigned index);

point get_free(point position, int size);
point s2a(point v, int size);
point s2a(point v, int size);

inline short unsigned i2x(short unsigned v) { return v & 0xFF; }
inline short unsigned i2y(short unsigned v) { return v >> 8; }
inline short unsigned m2i(int x, int y) { return (y << 8) + x; }

short unsigned get_free_index(short unsigned index, int radius, int size);
unsigned short s2i(point v);
unsigned short s2i(point v, int size);
short unsigned to(unsigned short index, directionn d);

color get_shadow(point s);

const sprite* get_minimap();
const sprite* get_area_sprites();

void block_impassable(short unsigned free_state);
void clear_area();
void clear_path_map();
bool is_block(short unsigned index);
bool is_block(short unsigned index, int size);
void read_area(const char* name);
void setcamera(point v);
void set_tile(short unsigned index, short unsigned tile);

namespace map {

int gettile(short unsigned index);

point getposition(short unsigned index, int size);

void set(unsigned short index, areaf_s v);

color* getpallette();

bool is(unsigned short index, areaf_s v);

}