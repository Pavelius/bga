#include "bsdata.h"
#include "creature.h"
#include "draw.h"
#include "order.h"

enum orderfn : unsigned char {
	OrderAction,
};

struct orderi {
	void*		parent;
	void*		object;
	fnevent		apply;
	unsigned char flags;
	explicit operator bool() const { return apply != 0; }
	void		clear();
	bool		is(orderfn v) const { return (flags & (1 << v)) != 0; }
	void		set(orderfn v) { flags |= 1 << v; }
};

static orderi last_order;

void orderi::clear() {
	memset(this, 0, sizeof(*this));
}

void clear_orders(void* parent) {
	last_order.clear();
}

void add_order(void* parent, void* object, fnevent apply) {
	last_order.apply = apply;
	last_order.parent = parent;
	last_order.object = object;
	last_order.flags = 0;
}

static void execute_order() {
	auto push_player = player;
	player = (creature*)draw::hot.param;
	((fnevent)draw::hot.param2)();
	player = push_player;
}

void update_orders() {
	if(!last_order.is(OrderAction))
		return;
	draw::execute(execute_order, (long)last_order.parent, (long)last_order.apply, last_order.object);
	last_order.clear();
}

void activate_order(void* parent) {
	if(last_order.parent != parent)
		return;
	last_order.set(OrderAction);
}