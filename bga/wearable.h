#include "item.h"

#pragma once

struct wearable {
	unsigned	coins;
	unsigned char weapon_index;
	int			weight, allowed_weight;
	item		wears[LastQuickItem + 1];
	void		addcoins(unsigned v);
	void		additem(item& v);
	slice<item>	backpack() { return slice<item>(wears + Backpack, wears + LastBackpack + 1); }
	void		equip(item& v);
	slice<item>	equipment() { return slice<item>(wears + Head, wears + LastQuickWeapon + 1); }
	int			getencumbrance() const;
	item&		getweapon() { return wears[QuickWeapon + weapon_index * 2]; }
	const item&	getweapon() const { return wears[QuickWeapon + weapon_index * 2]; }
	item&		getoffhand() { return wears[QuickOffhand + weapon_index * 2]; }
	const item& getoffhand() const { return wears[QuickOffhand + weapon_index * 2]; }
	bool		have(const item* p) const { return p >= wears && p < wears + sizeof(wears) / sizeof(wears[0]); }
	bool		useoffhand() const;
};