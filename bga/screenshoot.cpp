#include "draw.h"
#include "screenshoot.h"

using namespace draw;

screenshoot::screenshoot(rect rc, bool fade) : surface(rc.width(), rc.height(), getbpp()) {
	x = rc.x1;
	y = rc.y1;
	if(draw::canvas) {
		blit(*this, 0, 0, width, height, 0, *draw::canvas, x, y);
		if(fade) {
			pushrect push;
			auto push_canvas = canvas;
			auto push_clip = clipping;
			auto push_alpha = alpha;
			auto push_fore = fore;
			caret = point(0, 0); draw::width = this->width; draw::height = this->height;
			canvas = this;
			setclip();
			alpha = 128;
			fore = colors::form;
			rectf();
			fore = push_fore;
			alpha = push_alpha;
			clipping = push_clip;
			canvas = push_canvas;
		}
	}
}

screenshoot::screenshoot(bool fade) : screenshoot({0, 0, getwidth(), getheight()}, fade) {
}

screenshoot::~screenshoot() {
}

void screenshoot::restore() {
	setclip();
	if(draw::canvas)
		blit(*draw::canvas, x, y, width, height, 0, *this, 0, 0);
}

long open_dialog(fnevent proc, bool faded) {
	screenshoot push(faded);
	while(ismodal()) {
		push.restore();
		paintstart();
		proc();
		paintfinish();
		domodal();
	}
	return getresult();
}

void open_dialog() {
	open_dialog((fnevent)hot.object, hot.param);
}