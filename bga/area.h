#include "color.h"
#include "point.h"
#include "stringbuilder.h"
#include "slice.h"
#include "variable.h"

#pragma once

enum areafn : unsigned char {
	StateExplored,
};
enum directionn : unsigned char;

const short unsigned Blocked = 0xFFFF;

struct sprite;

struct areai {
	char			name[12];
	char			folder[4];
	slice<variable>	variables;
	static int compare(const void* v1, const void* v2);
	static areai* add(const char* name, const char* folder);
	static areai* find(const char* name, const char* folder);
};
extern short unsigned current_area;

inline short unsigned i2x(short unsigned v) { return v & 0xFF; }
inline short unsigned i2y(short unsigned v) { return v >> 8; }
inline short unsigned m2i(int x, int y) { return (y << 8) + x; }
inline short unsigned s2i(point v) { return (v.y / 12) * 256 + v.x / 16; }
inline point i2s(short unsigned v) { return point(i2x(v) * 16, i2y(v) * 12); }

extern char	area_name[12];
extern color area_light_pallette[256];
extern unsigned char area_zmap[256 * 256];
extern unsigned char area_light[256 * 256];
extern unsigned char area_state[256 * 256];
extern short unsigned area_tiles[64 * 64];
extern short unsigned area_cost[256 * 256];
extern short unsigned area_width, area_height, area_height_tiles;

unsigned char get_look(point s, point d); // Determine orientation

point a2s(point v, int size);
point get_free(point position, int size);
point s2a(point v, int size);

short unsigned get_free_index(short unsigned index, int radius, int size);
short unsigned to(unsigned short index, directionn d);

color get_shadow(point s);

const sprite* get_minimap();
const sprite* get_area_sprites();

void block_impassable(short unsigned free_state);
void clear_area();
void clear_path_map();
bool is_block(short unsigned index);
bool is_block(short unsigned index, int size);
bool is_state(unsigned short index, areafn v);
void read_area(const char* name);
void setcamera(point v);
void set_state(unsigned short index, areafn v);
void set_tile(short unsigned index, short unsigned tile);