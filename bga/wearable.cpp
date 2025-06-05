#include "wearable.h"

unsigned wearable::coins;

void wearable::addcoins(unsigned v) {
	if(!v)
		return;
	coins += v;
}

void wearable::additem(item& v) {
	// Try add coins
	if(v.is(Coins)) {
		addcoins(v.getcostall());
		v.clear();
		return;
	}
	// Try stack existing item
	for(auto& e : backpack()) {
		if(!e)
			continue;
		e.add(v);
		if(!v) {
			last_item = &e;
			break;
		}
	}
	// Add new one
	if(v) {
		for(auto& e : backpack()) {
			if(!e) {
				e = v;
				last_item = &e;
				v.clear();
				break;
			}
		}
	}
}

void wearable::equip(item& v) {
	for(auto i = Head; i <= LastQuickItem; i = (wearn)(i + 1)) {
		if(wears[i])
			continue;
		if(!v.canequip(i))
			continue;
		wears[i] = v;
		last_item = &wears[i];
		v.clear();
		break;
	}
	if(v)
		additem(v);
}

//bool wearable::useitem(const itemi* pi) {
//	for(auto& v : backpack()) {
//		if(!v.is(pi))
//			continue;
//		v.use();
//		return true;
//	}
//	return false;
//}

bool wearable::useoffhand() const {
	return wears[QuickWeapon + weapon_index]
		&& !wears[QuickWeapon + weapon_index].geti().is(TwoHanded);
}

int	wearable::getencumbrance() const {
	auto w = allowed_weight / 3;
	auto n = allowed_weight;
	if(weight > n)
		return 3;
	n -= w;
	if(weight > n)
		return 2;
	n -= w;
	if(weight > n)
		return 1;
	return 0;
}

wearn wearable::getslot(const void* p) const {
	if(p >= wears && p < wears + lenghtof(wears))
		return (wearn)((const item*)p - wears);
	return Backpack;
}