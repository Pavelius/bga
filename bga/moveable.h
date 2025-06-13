#pragma once

#include "drawable.h"

struct moveable : drawable {
	short unsigned move_order;
	point move_start, move_stop;
	bool ismoving() const { return move_start && move_stop; }
	void movestep(int speed);
};