#include "bsdata.h"
#include "colorgrad.h"
#include "draw.h"

using namespace draw;

static surface pallette_bitmap;
color pallette[256];
short color_indecies[34]; // default_color;

static unsigned char hair_aasimar[] = {3, 5, 79, 81, 110, 4, 80, 111};
static unsigned char hair_dark[] = {79, 110, 5, 3, 0};
static unsigned char hair_gold[] = {0, 1, 2, 5, 6, 4};
static unsigned char hair_gray[] = {79, 110, 5, 6};
static unsigned char hair_normal[] = {6, 0, 2, 1, 4, 111, 80, 3, 81, 7, 82, 79};
static unsigned char hair_tiefling[] = {0, 1, 2, 4, 6, 7, 80, 18, 19, 20};
static unsigned char hair_elf_wood[] = {1, 2, 4, 7, 112, 114};

static unsigned char skin_normal[] = {107,8, 114, 9, 10, 85, 84, 12, 16, 15, 17, 108, 106, 113, 14, 13, 105, 83};

BSDATA(colorgrad) = {
	{"HairAasimar", hair_aasimar},
	{"HairDark", hair_dark},
	{"HairGold", hair_gold},
	{"HairGray", hair_gray},
	{"HairNormal", hair_normal},
	{"HairTiefling", hair_tiefling},
	{"HairWoodElf", hair_elf_wood},
	{"SkinNormal", skin_normal},
};
BSDATAF(colorgrad)

void set_color(color* pallette, int start, int index, int count) {
	memcpy(pallette + start, pallette_bitmap.ptr(0, index), count * sizeof(color));
}

void clear_indecies() {
	for(auto& i : color_indecies)
		i = -1;
}

void set_color(const char* id) {
	auto p = bsdata<colorgrad>::find(id);
	if(!p)
		return;
	clear_indecies();
	auto index = 0;
	for(auto i : p->indecies)
		color_indecies[index++] = i;
}

void initialize_colorgrad() {
	pallette_bitmap.read("art/mpalette.bmp");
}
