#pragma once

typedef void(*fnevent)();

enum orderfn : unsigned char {
	OrderAction,
};

struct orderi {
	void*			parent;
	void*			object;
	fnevent			apply;
	unsigned char	flags;
	explicit operator bool() const { return apply != 0; }
	bool			is(orderfn v) const { return (flags & (1 << v)) != 0; }
	void			set(orderfn v) { flags |= 1 << v; }
};

orderi* add_order(void* parent, void* object, fnevent apply);
orderi* find_active_order(const void* parent);

void update_orders();