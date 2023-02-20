#include "draw.h"
#include "scrolltext.h"

using namespace draw;

static void set_origin_value() {
	auto p = (scrolltext*)hot.object;
	p->setorigin(hot.param);
}

void scrolltext::setorigin(int v) {
	origin = v;
	invalidate();
}

void scrolltext::clear() {
	invalidate();
	origin = 0;
	maximum = 0;
}

void scrolltext::invalidate() {
	cashe_string = -1;
	cashe_origin = 0;
}

void scrolltext::input() {
	switch(hot.key) {
	case MouseWheelDown: execute(set_origin_value, origin + texth(), 0, this); break;
	case MouseWheelUp: execute(set_origin_value, origin - texth(), 0, this); break;
	default: break;
	}
}

void scrolltext::correct() {
	if(origin + perscreen > maximum)
		origin = maximum - perscreen;
	if(origin < 0)
		origin = 0;
}

void scrolltext::paint(const char* format) {
	if(!format)
		return;
	auto push_clipping = clipping;
	setclipall();
	if(cashe_string == -1) {
		auto push_height = height;
		auto push_width = width;
		textfs(format);
		width = push_width;
		maximum = height;
		height = push_height;
		perscreen = height;
		correct();
		caret.y -= origin;
		textf(format, cashe_origin, cashe_string);
	} else
		textf(format, cashe_origin, cashe_string);
	clipping = push_clipping;
}