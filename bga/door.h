#include "drawable.h"
#include "point.h"
#include "sliceu.h"
#include "variable.h"

#pragma once

struct doortile {
	short unsigned		index; // index = y*64 + x;
	short unsigned		open; // new tile index
	short unsigned		closed; // new tile index
};
struct door : drawable, variableid {
	unsigned char		cursor;
	rect				box;
	point				launch;
	point				position_alternate;
	sliceu<point>		open_points;
	sliceu<point>		close_points;
	sliceu<point>		search_open_points;
	sliceu<point>		search_close_points;
	sliceu<doortile>	tiles;
	void				clear() { memset(this, 0, sizeof(*this)); }
	int					getcursor() const { return 30; }
	sliceu<point>		getpoints() const { return isopen() ? open_points : close_points; }
	point				getposition() const { return position; }
	rect				getrect() const { return box; }
	bool				isopen() const { return getvar().is(Opened); }
	bool				isvisibleactive() const { return true; }
	void				use(bool open);
};
