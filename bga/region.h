#include "drawable.h"
#include "rect.h"
#include "variable.h"

#pragma once

enum regiontn {
	RegionTriger, RegionInfo, RegionTravel
};
struct region : drawable, variableid {
	regiontn	type;
	point		launch;
	point		use;
	rect		box;
	char		move_to_area[8];
	char		move_to_entrance[32];
	sliceu<point> points;
};
