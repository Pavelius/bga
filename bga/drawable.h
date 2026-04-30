#include "point.h"

#pragma once

extern void* hilite_object;

struct drawable {
	point		position;
	void		clear();
	bool		ishilite() const { return this == hilite_object; };
	void		setposition(point v) { position = v; }
};
extern drawable* last_object;

bool inside(point t, point* points, int count);