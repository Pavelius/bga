#include "bsdata.h"
#include "entrance.h"
#include "stringbuilder.h"

entrance* entrance::find(const char* id) {
	for(auto& e : bsdata<entrance>()) {
		if(equal(e.name, id))
			return &e;
	}
	return 0;
}