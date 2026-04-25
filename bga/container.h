#include "drawable.h"
#include "sliceu.h"

#pragma once

struct container : drawable {
	enum typen : unsigned char {
		None,
		Bag, Chest, Drawer, Pile, Table, Shelf, Altar, Nonvisible,
		Spellbook, Body, Barrel, Crate
	};
	typen			type;
	point			launch;
	rect			box;
	sliceu<point>	points;
	unsigned char	index() const;
};
extern container* last_container;
