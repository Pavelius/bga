#pragma once

#include "drawable.h"

enum actionn : unsigned char;

struct orderable {
	actionn			action;
	short unsigned	target;
	constexpr explicit operator bool() const { return action != (actionn)0; }
	void clear() { action = (actionn)0; target = 0xFFFF; }
};

struct moveable : drawable {
	orderable		order;
	short unsigned	order_move; // Order end position index.
	point			move_start, move_stop;
	short unsigned	position_index;
	bool ismoving() const { return move_start && move_stop; }
	void movestep(int speed);
};