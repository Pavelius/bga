#include "bsreq.h"
#include "colorgrad.h"
#include "crt.h"
#include "draw.h"
#include "draw_command.h"
#include "draw_gui.h"
#include "resinfo.h"
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
		command::execute(gui.id, gui.value);
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

static void pressed_colorgrad(int index) {
	auto push_caret = caret;
	if(hot.pressed && gui.hilited) {
		caret.x += 1;
		caret.y += 1;
	}
	auto push_palt = palt; palt = pallette;
	set_color(4, index);
	image(gres(res::COLGRAD), 0, ImagePallette);
	palt = push_palt;
	caret = push_caret;
}

static void color_picker() {
	auto color_index = color_indecies[gui.value];
	pressed_button();
	if(color_index == -1)
		return;
	pressed_colorgrad(color_index);
	interactive_execute();
}

static void cursor_paint() {
	auto cicle = cursor.cicle;
	if(cursor.id == res::CURSORS) {
		auto pressed = hot.pressed;
		if(pressed)
			cicle += 1;
	}
	image(hot.mouse.x, hot.mouse.y, gres(cursor.id), cicle, 0);
}

void initialize_ui() {
	pfinish = cursor_paint;
	cursor.set(res::CURSORS, 0);
	draw::syscursor(false);
}

BSDATA(widget) = {
	{"Background", background},
	{"Button", button},
	{"ButtonNT", button_no_text},
	{"ColorPicker", color_picker},
	{"Label", label},
};
BSDATAF(widget)