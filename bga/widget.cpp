#include "bsreq.h"
#include "crt.h"
#include "draw.h"
#include "draw_command.h"
#include "draw_gui.h"
#include "resid.h"
#include "script.h"
#include "widget.h"

using namespace draw;

static void background() {
	image(gui.res, gui.value, 0);
}

static const char* getname() {
	const char* pn = 0;
	if(gui.text)
		pn = gui.text;
	if(!pn && gui.id)
		pn = getnm(gui.id);
	if(!pn)
		pn = "";
	return pn;
}

static void interactive_execute() {
	if(hot.key == MouseLeft && !hot.pressed && gui.hilited)
		command::execute(gui.id);
}

static void pressed_button() {
	auto frame = gui.frames[0];
	auto pressed = hot.pressed && gui.hilited;
	if(pressed)
		frame = gui.frames[1];
	image(gui.res, frame, 0);
}

static void pressed_text() {
	auto push_caret = caret;
	if(hot.pressed && gui.hilited) {
		caret.x += 1;
		caret.y += 2;
	}
	texta(getname(), AlignCenterCenter);
	caret = push_caret;
}

static void button() {
	pressed_button();
	interactive_execute();
	pressed_text();
}

static void button_no_text() {
	pressed_button();
	interactive_execute();
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
	{"ButtonNT", button_no_text},
	{"Label", label},
};
BSDATAF(widget)