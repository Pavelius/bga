#include "drawable.h"
#include "variable.h"
#include "sliceu.h"

#pragma once

struct container : drawable, variableid {
	enum typen : unsigned char {
		None,
		Bag, Chest, Drawer, Pile, Table, Shelf, Altar, Nonvisible,
		Spellbook, Body, Barrel, Crate
	};
	typen			type;
	char			name[32];
	point			launch;
	rect			box;
	sliceu<point>	points;
};
extern container* last_container;

container* get_container(variable* p);
