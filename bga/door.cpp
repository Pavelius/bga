#include "area.h"
#include "door.h"

void door::use(bool open) {
	if(open) {
		for(auto& e : tiles)
			set_tile(e.index, e.open);
	} else {
		for(auto& e : tiles)
			set_tile(e.index, e.closed);
	}
	if(open)
		getvar().set(Opened);
	else
		getvar().remove(Opened);
}