#include "point.h"

#pragma once

enum rendern : unsigned char;

extern void* hilite_object;

struct drawable {
	point		position;
	void		clear();
	static int	compare(const void* v1, const void* v2);
	int			getpriority() const;
	bool		ishilite() const { return this == hilite_object; };
	void		setposition(point v) { position = v; }
};
extern drawable* last_object;

bool inside(point t, point* points, int count);