#include "adat.h"
#include "point.h"

#pragma once

extern rect last_screen, last_area;
extern struct drawable* hilite_drawable;

enum rendern : unsigned char;

struct drawable {
	point		position;
	void		clear();
	static int	compare(const void* v1, const void* v2);
	int			getpriority() const;
	bool		ishilite() const { return this == hilite_drawable; };
	void		setposition(point v) { position = v; }
};
typedef adat<drawable*, 512> drawablea;
extern drawablea objects;

bool inside(point t, point* points, int count);