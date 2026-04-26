#include "ambient.h"
#include "animation.h"
#include "archive.h"
#include "area.h"
#include "container.h"
#include "creature.h"
#include "direction.h"
#include "door.h"
#include "draw.h"
#include "entrance.h"
#include "rfiles.h"
#include "floattext.h"
#include "io_stream.h"
#include "math.h"
#include "rand.h"
#include "region.h"

// static directionn all_aroud[] = {Left, Right, Up, Down, LeftUp, LeftDown, RightUp, RightDown};

char area_name[12];
color area_light_pallette[256];
unsigned char area_zmap[256 * 256];
unsigned char area_state[256 * 256];
unsigned char area_light[256 * 256];
unsigned short area_tiles[64 * 64];
unsigned short area_width, area_height, area_height_tiles;

bool combat_mode;
short unsigned current_area = -1;
static rfpma* pma_area;
static rfpma* pma_minimap;

static const unsigned char orientations_5b5[25] = {
	6, 7, 8, 9, 10,
	5, 6, 8, 10, 11,
	4, 4, 0, 12, 12,
	3, 2, 0, 14, 13,
	2, 1, 0, 15, 14
};
static const unsigned char orientations_7b7[49] = {
	6, 7, 7, 8, 9, 9, 10,
	5, 6, 7, 8, 9, 10, 11,
	5, 5, 6, 8, 10, 11, 11,
	4, 4, 4, 0, 12, 12, 12,
	3, 3, 2, 0, 14, 13, 13,
	3, 2, 1, 0, 15, 14, 13,
	2, 1, 1, 0, 15, 15, 14,
};

void clear_area() {
	if(pma_area) {
		pma_area->release();
		pma_area = 0;
	}
	if(pma_minimap) {
		pma_minimap->release();
		pma_minimap = 0;
	}
	area_height = area_width = area_height_tiles = 0;
	memset(area_tiles, 0, sizeof(area_tiles));
	memset(area_zmap, 0, sizeof(area_zmap));
	memset(area_state, 0, sizeof(area_state));
	memset(area_light, 0, sizeof(area_light));
	memset(area_light_pallette, 0, sizeof(area_light_pallette));
	bsdata<ambient>::source.clear();
	bsdata<animation>::source.clear();
	bsdata<container>::source.clear();
	bsdata<door>::source.clear();
	bsdata<doortile>::source.clear();
	bsdata<region>::source.clear();
	bsdata<point>::source.clear();
	bsdata<floattext>::source.clear();
}

void set_tile(short unsigned index, short unsigned tile) {
	area_tiles[index] = tile;
}

unsigned char get_look(point s, point d) {
	const int osize = 7;
	int dx = d.x - s.x;
	int dy = d.y - s.y;
	int st = (2 * imax(iabs(dx), iabs(dy)) + osize - 1) / osize;
	if(!st)
		return 0;
	int ax = dx / st;
	int ay = dy / st;
	return orientations_7b7[(ay + (osize / 2)) * osize + ax + (osize / 2)];
}

color get_shadow(point v) {
	return area_light_pallette[area_light[s2i(v)]];
}

static void archive_bitmap(archive& e, unsigned char* output, int output_bpp, int scan_line, int width, int height, color* pal) {
	for(int i = 0; i < height; i++) {
		e.set(output, width * (output_bpp / 8));
		output += scan_line;
	}
	if(pal)
		e.set(pal, sizeof(color) * 256);
}

static unsigned long area_signature() {
	unsigned long n = 0;
	unsigned long r = 0;
	r += (++n) * sizeof(doortile);
	r += (++n) * sizeof(door);
	r += (++n) * sizeof(region);
	r += (++n) * sizeof(container);
	r += (++n) * sizeof(animation);
	r += (++n) * sizeof(ambient);
	return r;
}

bool archive_ard(iostream& file, bool writemode) {
	archive a(file, writemode);
	if(!a.signature("ARD"))
		return false;
	if(!a.signature(area_signature()))
		return false;
	// Area header
	a.set(area_name, 8);
	a.set(area_width);
	a.set(area_height); area_height_tiles = (area_height * 12 + 15) / 16;
	// Objects with variables
	if(!a.signature("OBJ"))
		return false;
	a.set(bsdata<container>::source);
	a.set(bsdata<door>::source);
	a.set(bsdata<region>::source);
	a.set(bsdata<animation>::source);
	a.set(bsdata<ambient>::source);
	a.set(bsdata<point>::source);
	a.set(bsdata<doortile>::source);
	// Tile maps
	if(!a.signature("BMP"))
		return false;
	archive_bitmap(a, (unsigned char*)area_tiles, 16, 64 * sizeof(area_tiles[0]), area_width / 4, area_height_tiles / 4, 0);
	archive_bitmap(a, area_light, 8, 256, area_width, area_height, area_light_pallette);
	archive_bitmap(a, area_state, 8, 256, area_width, area_height, 0);
	return true;
}

static bool load_tls_file(const char* name) {
	if(pma_area) {
		pma_area->release();
		pma_area = 0;
	}
	pma_area = find_image(name);
	return pma_area != 0;
}

static bool load_mmp_file(const char* name) {
	if(pma_minimap) {
		pma_minimap->release();
		pma_minimap = 0;
	}
	pma_minimap = find_image(ids(name, "MM"));
	return pma_minimap != 0;
}

static bool load_ard_file(areai* p) {
	current_area = -1;
	if(!p)
		return false;
	io::file file(p->url, StreamRead);
	if(!file)
		return false;
	file.seek(p->offset, SeekSet);
	clear_area();
	current_area = getbsi(p);
	return archive_ard(file, false);
}

void read_area(areai* area) {
	if(!load_ard_file(area))
		return;
	if(!load_tls_file(area->id))
		return;
	if(!load_mmp_file(area->id))
		return;
	if(!area->is(AreaVisited)) {
		char temp[64]; stringbuilder sb(temp);
		sb.add("areas/%1.inf", area->id);
		area_read(temp);
		area->set(AreaVisited);
	}
}

bool is_state(unsigned short index, areafn v) {
	return (area_state[index] & (0x80 >> v)) != 0;
}

void set_state(unsigned short index, areafn v) {
	area_state[index] |= (0x80 >> v);
}

bool is_block(short unsigned index) {
	//0 - Obstacle - impassable, light blocking (черный)
	//1 - Sand ? (burgandy)
	//2 - Wood (зеленый)
	//3 - Wood (коричневый)
	//4 - Stone - echo-ey (темно-синий)
	//5 - Grass - soft (purple)
	//6 - Water - passable (turquoise)
	//7 - Stone - hard (светло-серый)
	//8 - Obstacle - impassable, non light blocking (темно-серый)
	//9 - Wood(red)
	//10 - Wall - impassable (bright green)
	//11 - Water - passable (желтый)
	//12 - Water - impassable (синий)
	//13 - Roof - impassable (pink)
	//14 - Worldmap exit (светло-синий)
	//15 - Grass (белый)
	unsigned char a = area_state[index] & 0x0F;
	return a == 0 || a == 8 || a == 10 || a == 12 || a == 13;
}

const sprite* get_minimap() {
	return pma_minimap->get();
}

const sprite* get_area_sprites() {
	return pma_area->get();
}

point get_free(point position, int size) {
	int i = s2i(a2s(position, size));
	if(!is_block(i, size))
		return s2a(i2s(i), size);
	return s2a(i2s(get_free_index(i, 1, size)), size);
}

short unsigned to(short unsigned index, directionn d) {
	switch(d) {
	case Left:
		if((index & 0xFF) == 0)
			return Blocked;
		return index - 1;
	case LeftUp:
		if((index & 0xFF) == 0)
			return Blocked;
		if((index >> 8) == 0)
			return Blocked;
		return index - 1 - 256;
	case Up:
		if((index >> 8) == 0)
			return Blocked;
		return index - 256;
	case RightUp:
		if((index & 0xFF) >= area_width - 1)
			return Blocked;
		if((index >> 8) == 0)
			return Blocked;
		return index - 256 + 1;
	case Right:
		if((index & 0xFF) >= area_width - 1)
			return Blocked;
		return index + 1;
	case RightDown:
		if((index & 0xFF) >= area_width - 1)
			return Blocked;
		if((index >> 8) >= area_height - 1)
			return Blocked;
		return index + 256 + 1;
	case Down:
		if((index >> 8) >= area_height - 1)
			return Blocked;
		return index + 256;
	case LeftDown:
		if((index & 0xFF) == 0)
			return Blocked;
		if((index >> 8) >= area_height - 1)
			return Blocked;
		return index - 1 + 256;
	default:
		return Blocked;
	}
}

static bool get_free_space_x(short unsigned& index, int radius, int size) {
	short unsigned px = index & 0xFF;
	short unsigned py = index >> 8;
	int minx = imax(px - radius, 0);
	int maxx = imin(px + radius + 1, (int)area_width);
	for(short unsigned scanx = minx; scanx < maxx; scanx++) {
		if(py >= radius) {
			auto i = m2i(scanx, py - radius);
			if(!is_block(i, size)) {
				index = i;
				return true;
			}
		}
		if(py + radius < area_height) {
			int i = m2i(scanx, py + radius);
			if(!is_block(i, size)) {
				index = i;
				return true;
			}
		}
	}
	return false;
}

static bool get_free_space_y(short unsigned& index, int radius, int size) {
	int px = index & 0xFF;
	int py = index >> 8;
	int miny = imax(py - radius, 0);
	int maxy = imin(py + radius + 1, (int)area_height);
	for(int scany = miny; scany < maxy; scany++) {
		if(px >= radius) {
			int i = m2i(px - radius, scany);
			if(!is_block(i, size)) {
				index = i;
				return true;
			}
		}
		if(px + radius < area_width) {
			int i = m2i(px + radius, scany);
			if(!is_block(i, size)) {
				index = i;
				return true;
			}
		}
	}
	return false;
}

short unsigned get_free_index(short unsigned index, int radius, int size) {
	if(is_block(index, size)) {
		int maxr = area_width / 2;
		if(maxr > area_height)
			maxr = area_height;
		for(; radius < maxr; radius++) {
			if(rand() & 1) {
				if(get_free_space_x(index, radius, size))
					break;
				if(get_free_space_y(index, radius, size))
					break;
			} else {
				if(get_free_space_y(index, radius, size))
					break;
				if(get_free_space_x(index, radius, size))
					break;
			}
		}
	}
	return index;
}

bool is_block(short unsigned index, int size) {
	if(size <= 1)
		return is_block(index);
	auto x1 = i2x(index);
	auto y1 = i2y(index);
	auto x2 = x1 + size;
	auto y2 = y1 + size;
	if(x2 >= area_width || y2 >= area_height)
		return true;
	for(auto y = y1; y < y2; y++) {
		for(auto x = x1; x < x2; x++) {
			if(is_block(m2i(x, y)))
				return true;
		}
	}
	return false;
}

static bool is_passable(short unsigned i0, short unsigned i1, int size) {
	int x0 = i2x(i0), y0 = i2x(i0);
	int x1 = i2x(i1), y1 = i2x(i1);
	int dx = iabs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -iabs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2;
	for(;;) {
		e2 = 2 * err;
		if(e2 >= dy) {
			if(x0 == x1)
				break;
			err += dy;
			if(e2 <= dx) {
				if(is_block(m2i(x0, y0 + sy), size))
					return false;
			}
			x0 += sx;
			if(is_block(m2i(x0, y0), size))
				return false;
		}
		if(e2 <= dx) {
			if(y0 == y1)
				break;
			err += dx;
			if(2 * err >= dy) {
				if(is_block(m2i(x0 + sx, y0), size))
					return false;
			}
			y0 += sy;
			if(is_block(m2i(x0, y0), size))
				return false;
		}
	}
	return true;
}

areai* get_area() {
	if(current_area == 0xFFFF)
		return 0;
	return bsdata<areai>::elements + current_area;
}