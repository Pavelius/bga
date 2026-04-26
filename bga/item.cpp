#include "flagable.h"
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
item* party_item;
item* store_item;

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

bool item::is(wearn v) const {
	v = get_slot(v);
	if(v == Backpack)
		return true;
	auto ew = geti().wear;
	switch(ew) {
	case LeftFinger: return v == LeftFinger || v == RightFinger;
	case Backpack: return true;
	default: return ew == v;
	}
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
	if(!v)
		return;
	if(!equal(v))
		return;
	auto max_count = 200;
	auto new_count = count + v.count;
	if(new_count <= max_count) {
		v.count = 0;
		count = new_count;
	} else {
		v.count = new_count - max_count;
		count = max_count;
	}
}

bool item::canequip(wearn v) const {
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

void item::setcount(int v) {
	if(v <= 0)
		v = 0;
	count = v;
	if(!count)
		clear();
}

void initialize_items() {
	for(auto& e : bsdata<itemi>()) {
		if(e.basic) {
			if(!e.weight)
				e.weight = e.basic->weight;
			if(!e.required)
				e.required = e.basic->required;
			if(!e.flags)
				e.flags = e.basic->flags;
			if(!e.weapon)
				e.weapon = e.basic->weapon;
			if(!e.cost)
				e.cost = e.basic->cost;
			if(!e.avatar)
				e.avatar = e.basic->avatar;
			if(!e.equiped)
				e.equiped = e.basic->equiped;
			if(!e.wear)
				e.wear = e.basic->wear;
		}
		if(e.weapon) {
			e.weapon.damage.b += e.magic;
			e.weapon.bonus += e.magic;
		}
	}
}