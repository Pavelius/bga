#pragma once

#include "item.h"
#include "variant.h"

struct collectiona;
struct iteminside : item {
	variant	parent;
	void	clear();
};
void add_item(variant parent, item& it);
void load_items(collectiona& source, variant parent);
void load_player_items(collectiona& source);
