#include "bsdata.h"
#include "order.h"

BSDATAC(orderi, 64)

orderi* last_order;

static void remove_tail() {
	while(bsdata<orderi>::source.count) {
		auto& e = bsdata<orderi>::elements[bsdata<orderi>::source.count - 1];
		if(e)
			break;
		bsdata<orderi>::source.count--;
	}
}

void clear_order(orderi* p) {
	memset(p, 0, sizeof(*p));
	remove_tail();
}

orderi* add_order(void* parent, void* object, fnevent apply) {
	auto p = bsdata<orderi>::addz();
	p->apply = apply;
	p->parent = parent;
	p->object = object;
	return p;
}

orderi* find_active_order(const void* parent) {
	for(auto& e : bsdata<orderi>()) {
		if(e.parent == parent)
			return &e;
	}
	return 0;
}