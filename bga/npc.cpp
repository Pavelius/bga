#include "npc.h"
#include "rfiles.h"

sprite* npci::getres(int n) {
	if(resid[n])
		return resid[n]->get();
	return 0;
}