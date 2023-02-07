#include "point.h"

#pragma once

struct drawable : point {
	unsigned char	alpha, priority;
	point			getscreen() const;
};
struct moveable : drawable {
	unsigned short	frame, cicle;
	void			paint() const; // Exported function
};