#include "crt.h"
#include "rect.h"
#include "point.h"

#pragma once

struct doortile {
	short unsigned		index; // index = y*64 + x;
	short unsigned		open; // new tile index
	short unsigned		closed; // new tile index
};
struct door {
	unsigned char		cursor;
	rect				box;
	point				launch;
	point				points[2];
	sliceu<point>		open_points;
	sliceu<point>		close_points;
	sliceu<point>		search_open_points;
	sliceu<point>		search_close_points;
	sliceu<doortile>	tiles;
	bool				opened;
	bool				locked;
	void				clear() { memset(this, 0, sizeof(*this)); }
	int					getcursor() const { return 30; }
	sliceu<point>		getpoints() const { return isopen() ? open_points : close_points; }
	point				getposition() const { return points[0]; }
	rect				getrect() const { return box; }
	bool				isopen() const { return opened; }
	bool				isvisibleactive() const { return true; }
};
