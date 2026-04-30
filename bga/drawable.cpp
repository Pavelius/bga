#include "animation.h"
#include "drawable.h"
#include "door.h"
#include "itemground.h"
#include "region.h"
#include "screenshoot.h"

drawable* last_object;

void drawable::clear() {
	memset(this, 0, sizeof(*this));
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