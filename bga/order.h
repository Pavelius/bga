#pragma once

typedef void(*fnevent)();

// enum actionn : unsigned char;

struct orderi {
	void*	parent;
	void*	object;
	fnevent apply;
	explicit operator bool() const { return apply != 0; }
};
extern orderi* last_order;

void clear_order(orderi* p);

orderi* add_order(void* parent, void* object, fnevent apply);
orderi* find_active_order(const void* parent);