#include "area.h"
#include "crt.h"
#include "worldmap.h"

worldmapi* current_world;

static areai* find_area(const char* id) {
	for(auto& e : bsdata<areai>()) {
		if(strcmp(e.name, id) == 0)
			return &e;
	}
	return 0;
}

worldmapi::area* get_party_world_area() {
	for(auto& e : bsdata<worldmapi::area>()) {
		auto pa = find_area(e.id);
		if(!pa)
			continue;
		if(getbsi(pa) == current_area)
			return &e;
	}
	return 0;
}