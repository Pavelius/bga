#include "bsdata.h"
#include "wear.h"

BSDATA(weari) = {
	{"Backpack"},
	{"Backpack2"},
	{"Backpack3"},
	{"Backpack4"},
	{"Backpack5"},
	{"Backpack6"},
	{"Backpack7"},
	{"Backpack8"},
	{"Backpack9"},
	{"Backpack10"},
	{"Backpack11"},
	{"Backpack12"},
	{"Backpack13"},
	{"Backpack14"},
	{"Backpack15"},
	{"Backpack16"},
	{"Backpack17"},
	{"Backpack18"},
	{"Backpack19"},
	{"Backpack20"},
	{"Backpack21"},
	{"Backpack22"},
	{"Backpack23"},
	{"Backpack24"},
	{"Head"},
	{"Neck"},
	{"Body"},
	{"Rear"},
	{"LeftFinger"},
	{"RightFinger"},
	{"Hands"},
	{"Gridle"},
	{"Legs"},
	{"QuickWeapon"},
	{"QuickOffhand"},
	{"QuickWeapon2"},
	{"QuickOffhand2"},
	{"QuickWeapon3"},
	{"QuickOffhand3"},
	{"QuickWeapon4"},
	{"QuickOffhand4"},
	{"Quiver"},
	{"Quiver2"},
	{"Quiver3"},
	{"QuickItem"},
	{"QuickItem2"},
	{"QuickItem3"},
};
BSDATAF(weari)

wearn get_slot(wearn v) {
	if(v >= Backpack && v <= LastBackpack)
		return Backpack;
	else if(v >= Quiver && v <= LastQuiver)
		return Quiver;
	else if(v >= QuickItem && v <= LastQuickItem)
		return QuickItem;
	else if(v == LeftFinger || v == RightFinger)
		return LeftFinger;
	else if(v == QuickWeapon
		|| v == QuickWeapon + 2
		|| v == QuickWeapon + 4
		|| v == QuickWeapon + 6)
		return QuickWeapon;
	else if(v == QuickOffhand
		|| v == QuickOffhand + 2
		|| v == QuickOffhand + 4
		|| v == QuickOffhand + 6)
		return QuickOffhand;
	return v;
}