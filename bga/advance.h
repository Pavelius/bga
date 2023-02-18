#include "variant.h"

#pragma once

struct advancei {
	variant			parent;
	int				level;
	const char*		id;
	variants		elements;
};
