#include "drawable.h"
#include "variable.h"

#pragma once

struct container : drawable, variableid {
	enum type_s : unsigned char {
		None,
		Bag, Chest, Drawer, Pile, Table, Shelf, Altar, Nonvisible,
		Spellbook, Body, Barrel, Crate
	};
	type_s			type;
	char			name[32];
	point			launch;
	rect			box;
	sliceu<point>	points;
};
