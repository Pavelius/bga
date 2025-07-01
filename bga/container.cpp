#include "container.h"

container* last_container;

container* get_container(variable* p) {
	if(p->id[6] != 'C')
		return 0;
	return bsdata<container>::elements + p->index;
}