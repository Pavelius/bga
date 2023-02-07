#include "point.h"

#pragma once

struct moveable {
	point		position;
	void		paint() const; // Exported function
};