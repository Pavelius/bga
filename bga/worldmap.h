#pragma once

#include "geography.h"
#include "point.h"

struct residi;

enum worldf_s {
	AreaVisible = 1,
	AreaVisibleFromAdjanced = 2,
	AreaReachable = 4,
	AlreadyVisited = 8,
};
struct worldmapi : nameable {
	struct area : nameable {
		point			position;
		worldmapi*		realm;
		int				avatar;
		unsigned		flags;
		bool			isinteract() const { return avatar != -1; }
	};
	struct link {
		worldmapi::area* from;
		worldmapi::area* to;
		const char*		entry;
		geography_s		side;
		unsigned		time; // Travel hours = time * 4
		unsigned char	encounter_chance;
		unsigned		flags;
	};
	residi*				background;
	residi*				icons;
};
extern worldmapi*		current_world;
worldmapi::area*		get_party_world_area();
