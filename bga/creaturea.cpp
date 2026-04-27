#include "area.h"
#include "creaturea.h"

bool need_update_creatures;
creaturea creatures;

void update_creatures() {
	if(!need_update_creatures)
		return;
	need_update_creatures = false;
	for(auto& e : bsdata<creature>()) {
		if(!e || e.area_index != current_area)
			continue;
		creatures.add(&e);
	}
}