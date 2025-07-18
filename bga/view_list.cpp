#include "draw.h"
#include "view_list.h"

using namespace draw;

int list_row_index;

void correct_table(int& origin, int maximum, int per_page) {
	if(origin + per_page > maximum)
		origin = maximum - per_page;
	if(origin < 0)
		origin = 0;
}

void correct_table(int& current, int maximum) {
	if(current > maximum)
		current = maximum - 1;
	if(current < 0)
		current = 0;
}

void input_mouse_table(int& origin, int maximum, int per_page, int per_row) {
	if(!ishilite())
		return;
	switch(hot.key) {
	case MouseWheelUp: execute(cbsetint, origin - per_row, 0, &origin); break;
	case MouseWheelDown: execute(cbsetint, origin + per_row, 0, &origin); break;
	}
}