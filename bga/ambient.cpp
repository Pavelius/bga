#include "ambient.h"

int ambient::hearing(point camera) const {
	if(!radius || is(AmbientGlobal))
		return volume;
	else {
		auto i = distance(position, camera);
		if(i >= radius)
			return 0;
		return (100 - i * 100 / radius) * volume / 100;
	}
}