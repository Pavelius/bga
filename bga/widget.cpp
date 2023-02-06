#include "bsreq.h"
#include "crt.h"
#include "draw.h"
#include "draw_gui.h"
#include "resid.h"
#include "widget.h"

using namespace draw;

static void background() {
	image(gui.res, gui.value, 0);
}

static void button() {
	auto frame = gui.frames[0];
	auto pressed = hot.pressed && ishilite();
	if(pressed)
		frame = gui.frames[1];
	image(gui.res, frame, 0);
	auto pn = gui.id;
	if(pn) {
		auto push_caret = caret;
		if(pressed) {
			caret.x += 1;
			caret.y += 2;
		}
		texta(pn, AlignCenterCenter);
	}
}

static void label() {
}

BSDATA(widget) = {
	{"Background", background},
	{"Button", button},
	{"Label", label},
};
BSDATAF(widget)