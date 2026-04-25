#include "creature.h"
#include "itemground.h"
#include "itemlist.h"
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

void storeitemlist::update() {
	auto index = last_store->index();
	for(auto& e : bsdata<itemground>()) {
		if(e.inside(e.Store, index))
			add(&e);
	}
}

void partyitemlist::update() {
	clear();
	for(auto& e : player->backpack()) {
		if(e)
			add(&e);
	}
}