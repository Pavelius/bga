#include "bsdata.h"
#include "order.h"

BSDATAC(orderi, 256)

static orderi* add_new() {
	for(auto& e : bsdata<orderi>()) {
		if(!e)
			return &e;
	}
	return (orderi*)bsdata<orderi>::source.add();
}

static void remove_link_to_this(short unsigned link) {
	for(auto& e : bsdata<orderi>()) {
		if(e.next == link)
			e.clear();
	}
}

void addorder(variant parent, variant action) {
	auto p = add_new();
	p->action = action;
	p->parent = parent;
}

void orderi::clear() {
	memset(this, 0, sizeof(*this));
}

void orderi::remove() {
	if(!operator bool())
		return;
	clear();
	remove_link_to_this(getbsi(this));
}