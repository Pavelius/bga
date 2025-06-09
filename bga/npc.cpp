#include "npc.h"
#include "resid.h"

sprite* npci::getres(int n) {
	if(!sprites[n]) {
		if(!res[n])
			return 0;
		sprites[n] = gres(res[n], "art/monsters");
	}
	if(sprites[n])
		return sprites[n];
	return 0;
}