#include "bsdata.h"
#include "collection.h"
#include "iteminside.h"
#include "creature.h"

BSDATAC(iteminside, 8192)

void iteminside::clear() {
	memset(this, 0, sizeof(*this));
}

static void copy_object(item& t1, const item& t2) {
	t1 = t2;
}

void add_item(variant parent, item& it) {
	for(auto& v : bsdata<iteminside>()) {
		if(v.parent != parent)
			continue;
		v.add(it);
		if(!it)
			return;
	}
	for(auto& v : bsdata<iteminside>()) {
		if(v)
			continue;
		v.parent = parent;
		copy_object(v, it);
		it.clear();
		last_item = &v;
		return;
	}
	auto p = bsdata<iteminside>::add();
	copy_object(*p, it);
	p->parent = parent;
	last_item = p;
	it.clear();
}

void load_items(collectiona& source, variant parent) {
	source.clear();
	for(auto& e : bsdata<iteminside>()) {
		if(e.parent == parent)
			source.add(&e);
	}
}

void load_player_items(collectiona& source) {
	source.clear();
	for(auto& e : player->backpack()) {
		if(!e)
			continue;
		source.add(&e);
	}
}