#include "npc.h"
#include "rfiles.h"

sprite* npci::getres(int n) {
	if(res[n])
		return res[n]->get();
	return 0;
}