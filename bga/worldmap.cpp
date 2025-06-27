#include "area.h"
#include "bsdata.h"
#include "worldmap.h"

worldmapi* current_world;

worldmapi::area* get_party_world_area() {
	for(auto& e : bsdata<worldmapi::area>()) {
		auto pa = bsdata<areai>::find(e.id);
		if(!pa)
			continue;
		if(getbsi(pa) == current_area)
			return &e;
	}
	return 0;
}