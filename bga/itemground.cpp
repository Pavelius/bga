#include "itemground.h"

void itemground::clear() {
	memset((void*)this, 0, sizeof(*this));
}

void add_item(short unsigned area, point position, item& v) {
	// Add to existing item
	for(auto& e : bsdata<itemground>()) {
		if(e.area == area && e.position == position && e.equal(v)) {
			e.add(v);
			if(!v) {
				last_item = &e;
				return;
			}
		}
	}
	auto p = bsdata<itemground>::addz();
	p->type = v.type;
	p->count = v.count;
	p->data = v.data;
	p->area = area;
	p->position = position;
	v.clear();
	last_item = p;
}