#include "color.h"

#pragma once

enum colorationn : unsigned char {
	SkinColor, HairColor, MajorColor, MinorColor,
	ArmorColor, MetalColor, LeatherColor,
};
struct coloration {
	unsigned char	colors[LeatherColor + 1];
	void			setpallette(color* v) const;
	void			setcolor(unsigned char* v);
};
