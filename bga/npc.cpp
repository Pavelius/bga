#include "npc.h"
#include "resid.h"

sprite* npci::getres(int n) {
	if(sprites[n])
		return sprites[n];
	if(!res[n])
		return 0;
	sprites[n] = gres(res[n], "art/monsters");
	return sprites[n];
}

void clear_npc_sprites() {
	for(auto& e : bsdata<npci>()) {
		e.sprites[0] = 0;
		e.sprites[1] = 0;
	}
}