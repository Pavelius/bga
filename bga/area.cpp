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
#include "floattext.h"
#include "io_stream.h"
#include "itemground.h"
#include "math.h"
#include "rand.h"
#include "region.h"

static directionn all_aroud[] = {Left, Right, Up, Down, LeftUp, LeftDown, RightUp, RightDown};

char area_name[12];
color area_light_pallette[256];
unsigned char area_zmap[256 * 256];
unsigned char area_state[256 * 256];
unsigned char area_light[256 * 256];
unsigned short area_tiles[64 * 64];
unsigned short area_width, area_height, area_height_tiles;

bool combat_mode;

unsigned short current_area;

static sprite* area_sprites;
static sprite* area_minimap_sprites;

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

static void initialize_variable(variableid* pv, char type, int index) {
	char sym[2] = {type, 0};
	auto& e = pv->getvar();
	e.id = szdup(str("%1%2%3.2i", area_name, sym, index + 1));
}

template<class T> void initilalize_variables(char type) {
	int n = bsdata<T>::source.count;
	for(auto i = 0; i < n; i++)
		initialize_variable(bsdata<T>::elements + i, type, i);
}

static void add_area_header(const char* url) {
	io::file file(url, StreamRead);
	if(!file)
		return;
	clear_area();
	if(!archive_ard(file, false, false))
		return;
	auto p = bsdata<areai>::add();
	p->id = szdup(area_name);
	p->variables.setbegin();
	bsdata<variable>::source.count += variable_count;
	p->variables.setend();
	current_variable_base = p->variables.begin();
	initilalize_variables<animation>('A');
	initilalize_variables<ambient>('S');
	initilalize_variables<container>('C');
	initilalize_variables<door>('D');
	initilalize_variables<region>('R');
}

void initialize_area() {
	bsdata<areai>::source.clear();
	for(io::file::find file("art/area"); file; file.next()) {
		auto pn = file.name();
		if(pn[0] == '.')
			continue;
		if(!szpmatch(pn, "*.ard"))
			continue;
		char temp[260];
		add_area_header(file.fullname(temp));
	}
	clear_area();
}

void clear_area() {
	if(area_sprites) {
		delete area_sprites;
		area_sprites = 0;
	}
	if(area_minimap_sprites) {
		delete area_minimap_sprites;
		area_minimap_sprites = 0;
	}
	area_height = area_width = area_height_tiles = 0;
	variable_count = 0;
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
	bsdata<entrance>::source.clear();
	bsdata<itemground>::source.clear();
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

static const char* gmurl(char* temp, const char* name, const char* ext = 0, const char* suffix = 0) {
	stringbuilder sb(temp, temp + 259);
	sb.add("art/area/");
	sb.add(name);
	if(suffix)
		sb.add(suffix);
	if(!ext)
		ext = "ard";
	sb.add(".");
	sb.add(ext);
	return temp;
}

static void archive_bitmap(archive& e, unsigned char* output, int output_bpp, int scan_line, int width, int height, color* pal) {
	for(int i = 0; i < height; i++) {
		e.set(output, width * (output_bpp / 8));
		output += scan_line;
	}
	if(pal)
		e.set(pal, sizeof(color) * 256);
}

static unsigned long get_area_signature() {
	unsigned long n = 0;
	unsigned long r = 0;
	r += (++n) * sizeof(doortile);
	r += (++n) * sizeof(door);
	r += (++n) * sizeof(region);
	r += (++n) * sizeof(container);
	r += (++n) * sizeof(entrance);
	r += (++n) * sizeof(animation);
	r += (++n) * sizeof(ambient);
	return r;
}

bool archive_ard(iostream& file, bool writemode, bool content) {
	archive ar(file, writemode);
	if(!ar.signature("ARD"))
		return false;
	if(!ar.signature(get_area_signature()))
		return false;
	// Area header
	ar.set(area_name, 8);
	ar.set(area_width);
	ar.set(area_height); area_height_tiles = (area_height * 12 + 15) / 16;
	ar.set(variable_count);
	// Objects with variables
	ar.set(bsdata<container>::source);
	ar.set(bsdata<door>::source);
	ar.set(bsdata<region>::source);
	ar.set(bsdata<animation>::source);
	ar.set(bsdata<ambient>::source);
	if(content) {
		// Tile maps
		archive_bitmap(ar, (unsigned char*)area_tiles, 16, 64 * sizeof(area_tiles[0]), area_width / 4, area_height_tiles / 4, 0);
		archive_bitmap(ar, area_light, 8, 256, area_width, area_height, area_light_pallette);
		archive_bitmap(ar, area_state, 8, 256, area_width, area_height, 0);
		// Objects without variables
		ar.set(bsdata<point>::source);
		ar.set(bsdata<doortile>::source);
		ar.set(bsdata<entrance>::source);
	}
	return true;
}

static bool load_tls_file(const char* name) {
	char temp[260];
	if(area_sprites)
		delete area_sprites;
	area_sprites = (sprite*)loadb(gmurl(temp, name, "pma"));
	return area_sprites != 0;
}

static bool load_mmp_file(const char* name) {
	char temp[260];
	if(area_minimap_sprites)
		delete area_minimap_sprites;
	area_minimap_sprites = (sprite*)loadb(gmurl(temp, name, "pma", "MM"));
	return area_minimap_sprites != 0;
}

static bool load_ard_file(const char* name) {
	char temp[260];
	io::file file(gmurl(temp, name));
	if(!file)
		return false;
	clear_area();
	return archive_ard(file, false, true);
}

void read_area(const char* name) {
	//res::cleanup();
	if(!load_ard_file(name))
		return;
	if(!load_tls_file(area_name))
		return;
	if(!load_mmp_file(area_name))
		return;
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
	return area_minimap_sprites;
}

const sprite* get_area_sprites() {
	return area_sprites;
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

variable* find_var(const char* area_id, size_t index) {
	if(!area_id && current_area != -1)
		return bsdata<areai>::elements[current_area].variables.begin() + index;
	auto p = bsdata<areai>::find(area_id);
	if(!p)
		return 0;
	return p->variables.begin() + index;
}

void set_var(const char* id, short unsigned index, tagn value) {
	auto p = find_var(id, index);
	if(p)
		p->set(value);
}