#include "drawable.h"
#include "rect.h"
#include "sliceu.h"

#pragma once

enum regiontn {
	RegionTriger, RegionInfo, RegionTravel
};
struct region : drawable {
	regiontn type;
	point launch;
	point use;
	rect box;
	char move_to_entrance[16];
	sliceu<point> points;
};

bool intersect(const point& a, const point& b, int y, int& x);