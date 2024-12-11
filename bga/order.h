#pragma once

#include "drawable.h"
#include "variant.h"

struct orderi {
	variant			parent, action;
	short unsigned	next;
	explicit operator bool() const { return parent.type != 0; }
	void			clear();
	void			remove();
};

void addorder(variant parent, variant action);
