#include "drawable.h"
#include "item.h"

#pragma once

struct itemground : drawable, item {
	enum specialn : short { Container = -1, Store = -2}; // if `y` value have this, then `x` is index.
	short unsigned area;
	short unsigned flags;
	bool inside(specialn v) const { return position.y == v; }
	bool inside(specialn v, short index) const { return position.y == v && position.x == index; }
};
