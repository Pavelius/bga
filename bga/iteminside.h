#pragma once

#include "item.h"
#include "variant.h"

struct iteminside : item {
	variant	parent;
	void	clear();
};
void add_item(variant parent, item& it);
