#include "collection.h"
#include "point.h"

#pragma once

extern rect last_screen, last_area;
extern struct drawable* hilite_drawable;

struct drawable {
	point			position;
	unsigned char	alpha, priority, random, layer;
	constexpr explicit operator bool() const { return layer != 0; }
	void			clear();
	static int		compare(const void* v1, const void* v2);
	bool			ishilite() const { return this == hilite_drawable; };
};
extern collection<drawable> objects;
bool inside(point t, point* points, int count);