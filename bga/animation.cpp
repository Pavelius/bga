#include "animation.h"
#include "calendar.h"

bool animation::isvisible() const {
	return active_time(shedule);
}