#include "arcfile.h"
#include "color.h"
#include "flagable.h"
#include "nameable.h"
#include "point.h"
#include "stringbuilder.h"
#include "slice.h"

#pragma once

enum areaf : unsigned char {
	AreaScounted, AreaVisited,
	AreaOutdoor, AreaCity, AreaForest, AreaDungeon,
};
enum areablockn : short unsigned {
	PassableMaximum = 0xFFF0,
	Blocked, BlockedCreature, BlockedLeft, BlockedUp,
};
enum directionn : unsigned char;

struct iostream;
struct sprite;
struct rfsnd;

struct areai : arcfile {
	typedef flagable<2, unsigned> flag64;
	rfsnd*		music;
	flag64		doors_opened, doors_trapped, doors_locked;
	flag32		region_disabled;
	flag64		animate_disabled;
	flag32		flags;
	unsigned	explore[128 * 4];
	bool is(areaf v) const { return flags.is(v); }
	void set(areaf v) { flags.set(v); }
	void remove(areaf v) { flags.remove(v); }
};
extern short unsigned current_area;

inline short unsigned i2x(short unsigned v) { return v & 0xFF; }
inline short unsigned i2y(short unsigned v) { return v >> 8; }
inline short unsigned m2i(int x, int y) { return (y << 8) + x; }
inline short unsigned s2i(point v) { return m2i(v.x / 16, v.y / 12); }
inline point i2s(short unsigned v) { return point(i2x(v) * 16, i2y(v) * 12); }
inline point i2sc(short unsigned v) { return point(i2x(v) * 16 + 8, i2y(v) * 12 + 6); }

extern char	area_name[12];
extern color area_light_pallette[256];
extern unsigned char area_zmap[256 * 256];
extern unsigned char area_light[256 * 256];
extern unsigned char area_state[256 * 256];
extern short unsigned area_tiles[64 * 64];
extern short unsigned area_cost[256 * 256];
extern short unsigned area_width, area_height, area_height_tiles;
extern unsigned area_visible[128 * 4];
extern bool combat_mode;
extern bool need_update_visibility;

areai* get_area();
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

bool archive_ard(iostream& file, bool writemode);
void area_read(const char* url);
void block_creatures();
void block_creatures(fnvisible allow, bool keep);
void block_movement(int range);
void change_cost(short unsigned v1, short unsigned v2);
void clear_area();
void clear_path_map();
void create_wave(short unsigned start, int size);
bool is_block(short unsigned index);
bool is_block(short unsigned index, int size);
bool is_explored(unsigned* data, int x, int y);
void read_area(areai* area);
void setcamera(point v);
void set_tile(short unsigned index, short unsigned tile);
void update_visibility();