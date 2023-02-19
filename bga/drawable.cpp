#include "drawable.h"
#include "door.h"
#include "region.h"
#include "screenshoot.h"

collection<drawable> objects;
rect last_screen, last_area;
drawable* hilite_drawable;

void drawable::clear() {
	memset(this, 0, sizeof(*this));
}

int drawable::getpriority() const {
	if(bsdata<door>::have(this))
		return 3 * 5;
	else if(bsdata<region>::have(this))
		return 2 * 5;
	return 5 * 5;
}

int drawable::compare(const void* v1, const void* v2) {
	auto p1 = *((drawable**)v1);
	auto p2 = *((drawable**)v2);
	auto n1 = p1->getpriority();
	auto n2 = p2->getpriority();
	auto r1 = n1 / 5;
	auto r2 = n2 / 5;
	if(r1 != r2)
		return r1 - r2;
	if(p1->position.y != p2->position.y)
		return p1->position.y - p2->position.y;
	if(n1 != n2)
		return n1 - n2;
	if(p1->position.x != p2->position.x)
		return p1->position.x - p2->position.x;
	return p1 - p2;
}

bool inside(point t, point* points, int count) {
	int j, yflag0, yflag1, xflag0, index;
	bool inside_flag = false;
	const point* vtx0, *vtx1;

	if(count < 3)
		return false;

	index = 0;
	vtx0 = points + count - 1;
	yflag0 = (vtx0->y >= t.y);
	vtx1 = &points[index];

	for(j = count + 1; --j;) {
		yflag1 = (vtx1->y >= t.y);
		if(yflag0 != yflag1) {
			xflag0 = (vtx0->x >= t.x);
			if(xflag0 == (vtx1->x >= t.x)) {
				if(xflag0)
					inside_flag = !inside_flag;
			} else {
				if((vtx1->x -
					(vtx1->y - t.y) * (vtx0->x - vtx1->x) /
					(vtx0->y - vtx1->y)) >= t.x) {
					inside_flag = !inside_flag;
				}
			}
		}
		yflag0 = yflag1;
		vtx0 = vtx1;
		vtx1 = &points[++index];
	}
	return inside_flag;
}