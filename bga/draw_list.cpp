#include "crt.h"
#include "draw.h"
#include "draw_gui.h"
#include "draw_list.h"

using namespace draw;

static int list_maximum, list_perpage;
static int* list_current;

void draw::listk(int& current, int& origin, int perpage, int perline, bool focus_origin) {
	list_current = &current;
	if(current >= list_maximum)
		current = list_maximum - 1;
	if(current < 0)
		current = 0;
	if(focus_origin) {
		if(current < origin)
			origin = current;
		if(origin + perpage < current)
			origin = current - perpage;
	}
	switch(hot.key) {
	case MouseWheelUp:
		if(gui.hilited && origin)
			execute(cbsetint, origin - 1, 0, &origin);
		break;
	case MouseWheelDown:
		if(gui.hilited && origin + perpage < list_maximum - 1)
			execute(cbsetint, origin + 1, 0, &origin);
		break;
	case KeyUp:
		if(current)
			execute(cbsetint, current - 1, 0, &current);
		break;
	case KeyDown:
		if(current < list_maximum - 1)
			execute(cbsetint, current + 1, 0, &current);
		break;
	case KeyHome:
		if(current)
			execute(cbsetint, 0, 0, &current);
		break;
	case KeyEnd:
		if(current != list_maximum - 1)
			execute(cbsetint, list_maximum - 1, 0, &current);
		break;
	case KeyPageUp:
		if(current)
			execute(cbsetint, current - perpage, 0, &current);
		break;
	case KeyPageDown:
		execute(cbsetint, current + perpage, 0, &current);
		break;
	}
	if(list_maximum > origin + perpage + 1)
		list_maximum = origin + perpage + 1;
}