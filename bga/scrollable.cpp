#include "draw.h"
#include "draw_gui.h"
#include "scrollable.h"

using namespace draw;

scrollable* last_scroll;

static void set_origin_value() {
	auto p = (scrollable*)hot.object;
	p->setorigin(hot.param);
}

void scrollable::clear() {
	invalidate();
	origin = 0;
	maximum = 0;
	perscreen = 0;
	perline = 0;
}

void scrollable::correct() {
	if(origin + perscreen > maximum)
		origin = maximum - perscreen;
	if(origin < 0)
		origin = 0;
}

void scrollable::setorigin(int v) {
	origin = v;
	correct();
}

void scrollable::keyinput() {
	switch(hot.key) {
	case KeyUp: break;
	case KeyDown: break;
	case KeyPageUp: execute(set_origin_value, origin - perscreen, 0, this); break;
	case KeyPageDown: execute(set_origin_value, origin + perscreen, 0, this); break;
	case KeyHome: execute(set_origin_value, 0, 0, this); break;
	case KeyEnd: execute(set_origin_value, origin + maximum - perscreen, 0, this); break;
	default: break;
	}
}

void scrollable::input() {
	switch(hot.key) {
	case MouseWheelDown: setcommand(origin + perline); break;
	case MouseWheelUp: setcommand(origin - perline); break;
	default: break;
	}
}

void scrollable::paint() {
	if(isinvalidated())
		update();
	if(gui.hilited)
		input();
	last_scroll = this;
}

void scrollable::setcommand(int v) {
	draw::execute(set_origin_value, v, 0, this);
}

int scrollable::proportial(int max_height) const {
	auto max = maximum - perscreen;
	if(!max_height || max <= 0)
		return 0;
	return origin * max_height / max;
}

void scrolltext::invalidate() {
	scrollable::invalidate();
	cashe_origin = 0;
}

void scrolltext::paint(const char* format) {
	last_scroll = this;
	perline = texth();
	if(!format)
		return;
	auto push_clipping = clipping;
	setclipall();
	if(isinvalidated() || current_origin != origin) {
		auto push_height = height;
		auto push_width = width;
		textfs(format);
		width = push_width;
		maximum = height;
		height = push_height;
		perscreen = height;
		correct();
		caret.y -= origin;
		cashe_content = -1;
		current_origin = origin;
		textf(format, cashe_origin, cashe_content);
	} else
		textf(format, cashe_origin, cashe_content);
	clipping = push_clipping;
}