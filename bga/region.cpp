#include "region.h"

bool intersect(const point& a, const point& b, int y, int& x) {
	int y1 = a.y - y;
	int y2 = b.y - y;
	if(y1 * y2 > 0)
		return false;
	if(y1 == 0 && y2 == 0)
		return false;
	x = a.x + ((b.x - a.x) * y1) / (y1 - y2);
	return true;
}