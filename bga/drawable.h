#include "collection.h"
#include "point.h"

#pragma once

extern rect last_screen, last_area;
extern struct drawable* hilite_drawable;

struct drawable {
	point			position;
	void			clear();
	static int		compare(const void* v1, const void* v2);
	int				getpriority() const;
	bool			ishilite() const { return this == hilite_drawable; };
	void			setposition(point v) { position = v; }
};
extern collection<drawable> objects;
bool inside(point t, point* points, int count);