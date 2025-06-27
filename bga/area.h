#include "color.h"
#include "nameable.h"
#include "point.h"
#include "stringbuilder.h"
#include "sliceu.h"
#include "variable.h"

#pragma once

enum areafn : unsigned char {
	StateExplored, StateVisible,
};
enum areablockn : short unsigned {
	PassableMaximum = 0xFFF0,
	Blocked, BlockedCreature, BlockedLeft, BlockedUp,
};
enum directionn : unsigned char;

struct iostream;
struct sprite;

struct areai : nameable {
	sliceu<variable> variables;
};
extern short unsigned current_area;

inline short unsigned i2x(short unsigned v) { return v & 0xFF; }
inline short unsigned i2y(short unsigned v) { return v >> 8; }
inline short unsigned m2i(int x, int y) { return (y << 8) + x; }
inline short unsigned s2i(point v) { return m2i(v.x / 16, v.y / 12); }
inline point i2s(short unsigned v) { return point(i2x(v) * 16, i2y(v) * 12); }
inline point a2s(point v, int size) { return point(v.x - 8 * (size - 1), v.y - 6 * (size - 1)); }
inline point s2a(point v, int size) { return point(v.x + 8 * size, v.y + 6 * size); }

extern char	area_name[12];
extern color area_light_pallette[256];
extern unsigned char area_zmap[256 * 256];
extern unsigned char area_light[256 * 256];
extern unsigned char area_state[256 * 256];
extern short unsigned area_tiles[64 * 64];
extern short unsigned area_cost[256 * 256];
extern short unsigned area_width, area_height, area_height_tiles;
extern bool combat_mode;

unsigned char get_look(point s, point d); // Determine orientation

point get_free(point position, int size);
point s2a(point v, int size);

short unsigned get_cost(short unsigned index);
short unsigned get_free_index(short unsigned index, int radius, int size);
short unsigned nearest_cost(short unsigned target);
short unsigned nearest_index(short unsigned target);
short unsigned nearest_index(short unsigned target, int range);
short unsigned to(short unsigned index, directionn d);

color get_shadow(point s);

const sprite* get_minimap();
const sprite* get_area_sprites();

variable* find_var(const char* area_id, size_t index);

bool archive_ard(iostream& file, bool writemode, bool content);
void block_creatures();
void block_creatures(fnvisible allow, bool keep);
void block_movement(int range);
void change_cost(short unsigned v1, short unsigned v2);
void clear_area();
void clear_path_map();
void create_wave(short unsigned start, int size);
void initialize_area();
bool is_block(short unsigned index);
bool is_block(short unsigned index, int size);
bool is_state(short unsigned index, areafn v);
void read_area(const char* name);
void setcamera(point v);
void set_state(short unsigned index, areafn v);
void set_tile(short unsigned index, short unsigned tile);
void set_var(const char* id, short unsigned index, tagn value);