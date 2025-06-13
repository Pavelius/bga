#include "point.h"
#include "math.h"

bool point::in(const point p1, const point p2, const point p3) const {
	int a = (p1.x - x) * (p2.y - p1.y) - (p2.x - p1.x) * (p1.y - y);
	int b = (p2.x - x) * (p3.y - p2.y) - (p3.x - p2.x) * (p2.y - y);
	int c = (p3.x - x) * (p1.y - p3.y) - (p1.x - p3.x) * (p3.y - y);
	return (a >= 0 && b >= 0 && c >= 0)
		|| (a < 0 && b < 0 && c < 0);
}

point center(const rect& rc) {
	return point(rc.x1 + rc.width() / 2, rc.y1 + rc.height() / 2);
}

rect create_rect(point p1, point p2) {
	rect r;
	if(p1.x < p2.x) {
		r.x1 = p1.x;
		r.x2 = p2.x;
	} else {
		r.x1 = p2.x;
		r.x2 = p1.x;
	}
	if(p1.y < p2.y) {
		r.y1 = p1.y;
		r.y2 = p2.y;
	} else {
		r.y1 = p2.y;
		r.y2 = p1.y;
	}
	return r;
}

int distance(point p1, point p2) {
	auto dx = p1.x - p2.x;
	auto dy = p1.y - p2.y;
	return isqrt(dx * dx + dy * dy);
}