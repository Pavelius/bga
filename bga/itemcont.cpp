#include "collection.h"
#include "crt.h"
#include "itemcont.h"
#include "creature.h"

BSDATAC(itemcont, 8192)

void itemcont::clear() {
	memset(this, 0, sizeof(*this));
}

static void copy_object(item& t1, const item& t2) {
	t1 = t2;
}

void add_item(variant parent, item& it) {
	for(auto& v : bsdata<itemcont>()) {
		if(v.parent != parent)
			continue;
		v.add(it);
		if(!it)
			return;
	}
	for(auto& v : bsdata<itemcont>()) {
		if(v)
			continue;
		v.parent = parent;
		copy_object(v, it);
		it.clear();
		last_item = &v;
		return;
	}
	auto p = bsdata<itemcont>::add();
	copy_object(*p, it);
	p->parent = parent;
	last_item = p;
	it.clear();
}

void load_items(collectiona& source, variant parent) {
	source.clear();
	for(auto& e : bsdata<itemcont>()) {
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