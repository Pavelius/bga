#include "item.h"

BSDATA(itemfi) = {
	{"TwoHanded"},
	{"Balanced"},
	{"UltraBalanced"},
	{"Dangerous"},
	{"Cursed"},
	{"BrilliantEnergy"},
	{"Disruption"},
	{"Distance"},
	{"Flaming"},
	{"FlamingBurst"},
	{"Frost"},
	{"GhostTouch"},
	{"Holy"},
	{"IcyBurst"},
	{"Keen"},
	{"Seeking"},
	{"Shock"},
	{"ShockingBurst"},
	{"Speed"},
	{"Unholy"},
	{"Vicious"},
	{"Vorpal"},
	{"Wounding"},
	{"Coins"},
};
assert_enum(itemfi, Coins)

item* last_item;

void weaponi::clear() {
	memset(this, 0, sizeof(*this));
}

const char* item::getname() const {
	return getnm(geti().id);
}

int itemi::getcritical() const {
	auto v = 0;
	if(FGT(flags, Balanced))
		v++;
	if(FGT(flags, UltraBalanced))
		v += 2;
	if(FGT(flags, Keen))
		v += 2;
	return 20 - v;
}

int itemi::getmultiplier() const {
	auto v = 2;
	if(FGT(flags, Dangerous))
		v++;
	return v;
}

int	item::getcostall() const {
	auto& ei = geti();
	auto cost = ei.cost * count;
	auto default_count = ei.count;
	if(default_count)
		cost = cost / default_count;
	return cost;
}

void item::add(item& v) {
	if(type != v.type || data != v.data)
		return;
	auto new_count = count + (1 + v.count);
	if(new_count <= 255) {
		v.count = 0;
		count = new_count;
	} else {
		v.count = new_count - 255;
		count = 255;
	}
}

bool item::canequip(wear_s v) const {
	switch(v) {
	case QuickWeapon + 2 * 0:
	case QuickWeapon + 2 * 1:
	case QuickWeapon + 2 * 2:
	case QuickWeapon + 2 * 3:
		return geti().wear == QuickWeapon;
	case QuickOffhand + 2 * 0:
	case QuickOffhand + 2 * 1:
	case QuickOffhand + 2 * 2:
	case QuickOffhand + 2 * 3:
		return geti().wear == QuickOffhand;
	case Quiver + 0:
	case Quiver + 1:
	case Quiver + 2:
		return geti().wear == Quiver;
	case QuickItem + 0:
	case QuickItem + 1:
	case QuickItem + 2:
		return geti().wear == QuickItem;
	case LeftFinger:
	case RightFinger:
		return geti().wear == LeftFinger;
	default:
		return geti().wear == v;
	}
}