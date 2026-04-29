#pragma once

#include "drawable.h"
#include "variant.h"

enum actionn : unsigned char;

//struct container;
//struct creature;
//struct door;
//struct region;
//
//struct orderable {
//	enum targetn : unsigned char { NoType, Creature, Container, Door, Region };
//	actionn			action; // Action occurs when order ends.
//	unsigned char	type; // Miscelaous action parameters.
//	short unsigned	target; // Can be container, creature, e.t.c. Depend on action.
//	void			clear() { action = (actionn)0; type = NoType; target = 0; }
//	template<typename T> operator T*() const;
//	template<typename T> void operator=(const T* v);
//};
struct moveable : drawable {
	variant			order;
	short unsigned	order_move; // Order end position index.
	point			move_start, move_stop;
	short unsigned	position_index;
	bool ismoving() const { return move_start && move_stop; }
	void movestep(int speed);
};