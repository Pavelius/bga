#pragma once

#include "nameable.h"
#include "point.h"

enum geography_s : unsigned char {
	North, East, South, West
};
enum worldf_s {
	AreaVisible = 1,
	AreaVisibleFromAdjanced = 2,
	AreaReachable = 4,
	AlreadyVisited = 8,
};

struct worldmapi : nameable {
	struct link {
		geography_s		side;
		unsigned char	index;
		const char*		entry;
		unsigned		time;
		unsigned char	encounter_chance;
		unsigned		flags;
	};
	const char*		realm;
	point			position;
	int				avatar;
	unsigned		flags;
};
