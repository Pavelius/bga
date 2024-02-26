#include "creature.h"
#include "itemlist.h"
#include "itemcont.h"
#include "store.h"

// itemlist* last_itemstore;

void itemlist::add(item* pi) {
	auto p = data + maximum;
	if(maximum < maximum_count)
		maximum++;
	p->data = pi;
	p->count = 0;
}

void itemlist::clear() {
	maximum = 0;
}

void itemlist::invalidate() {
	is_updated = false;
}

void storeitemlist::update() {
	variant parent = last_store;
	for(auto& e : bsdata<itemcont>()) {
		if(e.parent == parent)
			add(&e);
	}
}

void partyitemlist::update() {
	for(auto& e : player->backpack()) {
		if(e)
			add(&e);
	}
}