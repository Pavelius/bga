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

static const char* getname() {
	if(!gui.id)
		return "";
	return getnm(gui.id);
}

static void button() {
	auto frame = gui.frames[0];
	auto pressed = hot.pressed && gui.hilited;
	if(pressed)
		frame = gui.frames[1];
	image(gui.res, frame, 0);
	auto pn = getname();
	auto push_caret = caret;
	if(pressed) {
		caret.x += 1;
		caret.y += 2;
	}
	texta(pn, AlignCenterCenter);
}

static void label() {
	auto push_font = font;
	if(gui.res)
		font = gui.res;
	texta(getname(), AlignCenterCenter);
	font = push_font;
}

BSDATA(widget) = {
	{"Background", background},
	{"Button", button},
	{"Label", label},
};
BSDATAF(widget)