#include "bsdata.h"
#include "spell.h"

spellbook* find_spellbook(short unsigned owner, short unsigned type) {
	for(auto& e : bsdata<spellbook>()) {
		if(e.owner == owner && e.type == type)
			return &e;
	}
	return 0;
}