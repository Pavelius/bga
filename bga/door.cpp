#include "area.h"
#include "door.h"

void door::use(bool open) {
	if(open) {
		for(auto& e : tiles)
			map::settile(e.index, e.open);
	} else {
		for(auto& e : tiles)
			map::settile(e.index, e.closed);
	}
	opened = open;
}