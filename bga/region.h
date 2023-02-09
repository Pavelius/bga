#include "crt.h"
#include "point.h"
#include "rect.h"

#pragma once

enum region_type_s {
	RegionTriger, RegionInfo, RegionTravel
};
struct region {
	region_type_s	type;
	const char*		name;
	point			launch;
	point			use;
	rect			box;
	char			move_to_area[8];
	char			move_to_entrance[32];
	sliceu<point>	points;
	rect			getrect() const { return box; }
	int				getcursor() const;
	bool			isvisible() const { return type != RegionTriger; }
	void			painting(point screen) const {}
};
