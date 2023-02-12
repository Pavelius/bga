#include "color.h"

#pragma once

enum coloration_s : unsigned char {
	SkinColor, HairColor, MajorColor, MinorColor,
	ArmorColor, MetalColor, LeatherColor,
};

struct coloration {
	unsigned char	colors[7];
	void			setpallette(color* v) const;
	void			setcolor(unsigned char* v);
};
