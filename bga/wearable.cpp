#include "wearable.h"

void wearable::addcoins(unsigned v) {
	if(!v)
		return;
	coins += v;
}

void wearable::additem(item& v) {
	//if(v.is(Coins)) {
	//	addcoins(v.getcostall());
	//	v.clear();
	//	return;
	//}
	// Try stack existing item
	for(auto& e : backpack()) {
		if(!v)
			break;
		if(e)
			e.add(v);
	}
	if(v) {
		for(auto& e : backpack()) {
			if(!e) {
				e = v;
				v.clear();
				break;
			}
		}
	}
}

void wearable::equip(item& v) {
	for(auto i = Head; i <= LastQuickItem; i = (wear_s)(i + 1)) {
		if(wears[i])
			continue;
		if(!v.canequip(i))
			continue;
		wears[i] = v;
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