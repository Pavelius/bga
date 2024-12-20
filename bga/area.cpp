#include "animation.h"
#include "archive.h"
#include "area.h"
#include "container.h"
#include "creature.h"
#include "door.h"
#include "draw.h"
#include "entrance.h"
#include "floattext.h"
#include "io_stream.h"
#include "itemground.h"
#include "map.h"
#include "math.h"
#include "region.h"

static sprite* sprites;
static sprite* sprites_minimap;
unsigned char map::heightmap[256 * 256];
unsigned char map::statemap[256 * 256];
unsigned char map::lightmap[256 * 256];
unsigned short map::tilemap[64 * 64];
char map::areaname[12];
unsigned short current_area;
static color lightpal[256];

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

int	areai::compare(const void* v1, const void* v2) {
	return szcmp(((areai*)v1)->name, ((areai*)v2)->name);
}

areai* areai::add(const char* name, const char* folder) {
	auto p = find(name, folder);
	if(!p) {
		p = bsdata<areai>::add();
		stringbuilder s1(p->name); s1.add(name); s1.upper();
		stringbuilder s2(p->folder); s2.add(folder); s2.upper();
		auto pb = bsdata<variable>::end();
		bsdata<variable>::source.count += variable_count;
		auto pe = bsdata<variable>::end();
		p->variables = slice<variable>(pb, pe);
	}
	return p;
}

areai* areai::find(const char* name, const char* folder) {
	for(auto& e : bsdata<areai>()) {
		if(equal(e.name, name) && equal(e.folder, folder))
			return &e;
	}
	return 0;
}

color* map::getpallette() {
	return lightpal;
}

void map::clear() {
	if(sprites) {
		delete sprites;
		sprites = 0;
	}
	if(sprites_minimap) {
		delete sprites_minimap;
		sprites_minimap = 0;
	}
	initialize();
	height = width = height_tiles = 0;
	memset(tilemap, 0, sizeof(tilemap));
	memset(heightmap, 0, sizeof(heightmap));
	memset(statemap, 0, sizeof(statemap));
	memset(lightmap, 0, sizeof(lightmap));
	memset(lightpal, 0, sizeof(lightpal));
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

void map::settile(short unsigned index, short unsigned tile) {
	tilemap[index] = tile;
}

unsigned char map::getorientation(point s, point d) {
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

unsigned short map::getindex(point pos) {
	return (pos.y / 12) * 256 + pos.x / 16;
}

unsigned short map::getindex(point pos, int size) {
	return (pos.y / 12) * 256 + pos.x / 16;
}

point map::getposition(short unsigned index, int size) {
	return{(short)((index & 0xFF) * 16 + 8), (short)((((unsigned)index) >> 8) * 12 + 6)};
}

color map::getshadow(point s) {
	return lightpal[lightmap[getindex(s, 1)]];
}

unsigned char map::getstate(short unsigned index) {
	return statemap[index];
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

bool archive_ard(io::stream& file, bool writemode) {
	archive ar(file, writemode);
	if(!ar.signature("ARD"))
		return false;
	if(!ar.version(1, 0))
		return false;
	// ���������
	ar.set(map::width);
	ar.set(map::height); map::height_tiles = (map::height * 12 + 15) / 16;
	ar.set(map::areaname, 8);
	ar.set(variable_count);
	// ����� ������
	archive_bitmap(ar, (unsigned char*)map::tilemap, 16, 64 * sizeof(map::tilemap[0]), map::width / 4, map::height_tiles / 4, 0);
	archive_bitmap(ar, map::lightmap, 8, 256, map::width, map::height, lightpal);
	archive_bitmap(ar, map::statemap, 8, 256, map::width, map::height, 0);
	// �������
	ar.set(bsdata<point>::source);
	ar.set(bsdata<doortile>::source);
	ar.set(bsdata<door>::source);
	ar.set(bsdata<region>::source);
	ar.set(bsdata<container>::source);
	ar.set(bsdata<entrance>::source);
	ar.set(bsdata<animation>::source);
	return true;
}

static bool load_tls_file(const char* name) {
	char temp[260];
	if(sprites)
		delete sprites;
	sprites = (sprite*)loadb(gmurl(temp, name, "pma"));
	return sprites != 0;
}

static bool load_mmp_file(const char* name) {
	char temp[260];
	if(sprites_minimap)
		delete sprites_minimap;
	sprites_minimap = (sprite*)loadb(gmurl(temp, name, "pma", "MM"));
	return sprites_minimap != 0;
}

static bool load_ard_file(const char* name) {
	char temp[260];
	io::file file(gmurl(temp, name));
	if(!file)
		return false;
	map::clear();
	return archive_ard(file, false);
}

void map::read(const char* name) {
	//res::cleanup();
	if(!load_ard_file(name))
		return;
	if(!load_tls_file(areaname))
		return;
	if(!load_mmp_file(areaname))
		return;
	//worldmap::set(worldmap::getarea(name));
}

bool map::is(unsigned short index, areaf_s v) {
	return (statemap[index] & (0x80 >> v)) != 0;
}

void map::set(unsigned short index, areaf_s v) {
	statemap[index] |= (0x80 >> v);
}

bool map::isblock(short unsigned index) {
	//0 - Obstacle - impassable, light blocking (������)
	//1 - Sand ? (burgandy)
	//2 - Wood (�������)
	//3 - Wood (����������)
	//4 - Stone - echo-ey (�����-�����)
	//5 - Grass - soft (purple)
	//6 - Water - passable (turquoise)
	//7 - Stone - hard (������-�����)
	//8 - Obstacle - impassable, non light blocking (�����-�����)
	//9 - Wood(red)
	//10 - Wall - impassable (bright green)
	//11 - Water - passable (������)
	//12 - Water - impassable (�����)
	//13 - Roof - impassable (pink)
	//14 - Worldmap exit (������-�����)
	//15 - Grass (�����)
	if(is(index, CreatureBlock))
		return true;
	unsigned char a = statemap[index] & 0x0F;
	return a == 0 || a == 8 || a == 10 || a == 12 || a == 13;
}

const sprite* map::getminimap() {
	return sprites_minimap;
}

const sprite* map::getareasprite() {
	return sprites;
}

int map::gettile(short unsigned index) {
	return tilemap[index];
}

point map::getfree(point position, int size) {
	int i = getindex(position, size);
	if(!isblock(i, size))
		return position;
	return map::getposition(getfree(i, 1, size), size);
}