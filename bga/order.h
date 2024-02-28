#pragma once

#include "drawable.h"
#include "variant.h"

struct orderi : drawable {
	variant			parent, action;
	short unsigned	next;
	explicit operator bool() const { return parent.type != 0; }
	void			add(variant parent, variant action);
	void			add(point position, variant action, variant parent);
	void			clear();
	void			remove();
};
