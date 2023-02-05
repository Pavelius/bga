#include "item.h"

#pragma once

struct wearable {
	unsigned	coins;
	item		wears[LastQuickItem + 1];
	void		addcoins(unsigned v);
	void		additem(item& v);
	slice<item>	backpack() { return slice<item>(wears + Backpack, wears + LastBackpack + 1); }
	void		equip(item& v);
	bool		useitem(const itemi* pi);
};