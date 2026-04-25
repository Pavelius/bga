#include "drawable.h"
#include "item.h"

#pragma once

struct itemground : drawable, item {
	enum specialn : short { Container = -1, Store = -2};
	short unsigned area; // Area index where item lay. 0xFFFF is no use.
	void clear();
	bool inside() const { return position.y < 0; }
	bool inside(specialn v) const { return position.y == v; }
	bool inside(specialn v, short index) const { return position.y == v && position.x == index; }
};

void add_item(short unsigned area, point position, item& v);
