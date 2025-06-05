#include "nameable.h"

#pragma once

enum wearn : unsigned char {
	Backpack, LastBackpack = Backpack + 23,
	Head, Neck, Body, Rear, LeftFinger, RightFinger, Hands, Gridle, Legs,
	QuickWeapon, QuickOffhand, LastQuickWeapon = QuickWeapon + 4 * 2 - 1,
	Quiver, LastQuiver = Quiver + 2,
	QuickItem, LastQuickItem = QuickItem + 2,
};
struct weari : nameable {
	int		avatar;
};
wearn get_slot(wearn v);