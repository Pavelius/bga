#include "animation.h"
#include "calendar.h"

bool animation::isvisible() const {
	auto hour = gethour();
	if(schedule) {
		if((schedule & (1 << hour)) == 0)
			return false;
	}
	return true;
}