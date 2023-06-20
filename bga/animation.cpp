#include "animation.h"
#include "game.h"

bool animation::isvisible() const {
	auto hour = getgamehour();
	if(schedule) {
		if((schedule & (1 << hour)) == 0)
			return false;
	}
	return true;
}