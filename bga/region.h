#include "drawable.h"
#include "rect.h"

#pragma once

enum region_type_s {
	RegionTriger, RegionInfo, RegionTravel
};
struct region : drawable {
	region_type_s	type;
	point			launch;
	point			use;
	rect			box;
	char			move_to_area[8];
	char			move_to_entrance[32];
	sliceu<point>	points;
};
