#include "bsdata.h"
#include "creature.h"
#include "draw.h"
#include "order.h"

BSDATAC(orderi, 64)

static void remove_tail() {
	while(bsdata<orderi>::source.count) {
		auto& e = bsdata<orderi>::elements[bsdata<orderi>::source.count - 1];
		if(e)
			break;
		bsdata<orderi>::source.count--;
	}
}

static void clear_order(orderi* p) {
	memset(p, 0, sizeof(*p));
}

orderi* add_order(void* parent, void* object, fnevent apply) {
	auto p = bsdata<orderi>::addz();
	p->apply = apply;
	p->parent = parent;
	p->object = object;
	p->flags = 0;
	return p;
}

orderi* find_active_order(const void* parent) {
	for(auto& e : bsdata<orderi>()) {
		if(e.parent == parent)
			return &e;
	}
	return 0;
}

static void execute_order() {
	auto push_player = player;
	player = (creature*)draw::hot.param;
	((fnevent)draw::hot.param2)();
	player = push_player;
}

void update_orders() {
	for(auto& e : bsdata<orderi>()) {
		if(e.is(OrderAction)) {
			draw::execute(execute_order, (long)e.parent, (long)e.apply, e.object);
			clear_order(&e);
			break;
		}
	}
	remove_tail();
}