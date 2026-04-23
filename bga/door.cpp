#include "area.h"
#include "door.h"

bool door::isopen() const {
	auto index = getbsi(this);
	auto p = get_area();
	if(p)
		return p->doors_opened.is(index);
	return false;
}

void door::use(bool open) {
	auto index = getbsi(this);
	auto p = get_area();
	if(!p)
		return;
	if(open) {
		for(auto& e : tiles)
			set_tile(e.index, e.open);
	} else {
		for(auto& e : tiles)
			set_tile(e.index, e.closed);
	}
	if(open)
		p->doors_opened.set(index);
	else
		p->doors_opened.remove(index);
}