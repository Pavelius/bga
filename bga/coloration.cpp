#include "colorgrad.h"
#include "coloration.h"

void coloration::setpallette(color* col) const {
	set_color(col, 0x04, colors[MetalColor]); //metal
	set_color(col, 0x10, colors[MinorColor]); //minor
	set_color(col, 0x1C, colors[MajorColor]); //major
	set_color(col, 0x28, colors[SkinColor]); //skin
	set_color(col, 0x34, colors[LeatherColor]); //leather
	set_color(col, 0x40, colors[ArmorColor]); //armor
	set_color(col, 0x4C, colors[HairColor]); //hair
	memcpy(col + 0x58, col + 0x11, 8 * sizeof(color)); //minor
	memcpy(col + 0x60, col + 0x1d, 8 * sizeof(color)); //major
	memcpy(col + 0x68, col + 0x11, 8 * sizeof(color)); //minor
	memcpy(col + 0x70, col + 0x05, 8 * sizeof(color)); //metal
	memcpy(col + 0x78, col + 0x35, 8 * sizeof(color)); //leather
	memcpy(col + 0x80, col + 0x35, 8 * sizeof(color)); //leather
	memcpy(col + 0x88, col + 0x11, 8 * sizeof(color)); //minor
	// leather: bracers (90-97)
	for(int i = 0x90; i < 0xA8; i += 0x08)
		memcpy(col + i, col + 0x35, 8 * sizeof(color));
	// skin
	memcpy(col + 0xB0, col + 0x29, 8 * sizeof(color));
	// leather
	for(int i = 0xB8; i < 0xFF; i += 0x08)
		memcpy(col + i, col + 0x35, 8 * sizeof(color));
}

void coloration::setcolor(unsigned char* v) {
	colors[SkinColor] = v[0];
	colors[HairColor] = v[1];
	colors[MajorColor] = v[2];
	colors[MinorColor] = v[3];
	colors[MetalColor] = 28;
	colors[LeatherColor] = 23;
	colors[ArmorColor] = 30;
}