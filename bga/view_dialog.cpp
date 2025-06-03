#include "colorgrad.h"
#include "draw.h"
#include "pushvalue.h"
#include "resid.h"
#include "screenshoot.h"

using namespace draw;

static point dialog_start;
static bool button_pressed, button_executed, button_hilited, input_disabled;
static const char* header_id;

static void setdialog(int x, int y) {
	caret = dialog_start + point{(short)x, (short)y};
}

static void setdialog(int x, int y, int w, int h) {
	setdialog(x, y);
	width = w;
	height = h;
}

static void paint_dialog(resn v) {
	auto p = gres(v);
	auto& f = p->get(0);
	dialog_start.x = (getwidth() - f.sx) / 2;
	dialog_start.y = (getheight() - f.sy) / 2 - 128;
	if(dialog_start.y < 64)
		dialog_start.y = 64;
	caret = dialog_start;
	image(p, 0, 0);
}

static void input_cancel(int param = 0) {
	if(hot.key == KeyEscape)
		execute(buttonparam, param);
}

static void button_check(unsigned key) {
	static rect	button_rect;
	rect rc = {caret.x, caret.y, caret.x + width, caret.y + height};
	button_hilited = ishilite(rc);
	button_pressed = false;
	button_executed = false;
	if(input_disabled) {
		button_hilited = false;
		return;
	}
	if(button_hilited) {
		if(hot.pressed)
			button_pressed = true;
		if(!hot.pressed && hot.key == MouseLeft)
			button_executed = true;
	}
	if(key && hot.key == key)
		button_rect = rc;
	if(hot.key == InputKeyUp) {
		if(button_rect == rc) {
			button_executed = true;
			button_rect.clear();
		}
	}
	if(button_rect == rc)
		button_pressed = true;
}

static void fire(fnevent proc, long param = 0, long param2 = 0, const void* object = 0) {
	if(button_executed)
		execute(proc, param, param2, object);
}

static void button(resn res, unsigned short f1, unsigned short f2, unsigned key = 0) {
	auto p = gres(res);
	auto& f = p->get(f1);
	width = f.sx; height = f.sy;
	button_check(key);
	image(p, button_pressed ? f2 : f1, 0);
}

static void button(resn res, unsigned short f1, unsigned short f2, unsigned key, const char* id) {
	auto push_caret = caret;
	button(res, f1, f2, key);
	if(button_pressed) {
		caret.x++;
		caret.y++;
	}
	texta(str(getnm(id)), AlignCenterCenter);
	caret = push_caret;
}

static void button_colorgrad(int index, int size) {
	rectpush push;
	if(hot.pressed && button_hilited) {
		caret.x += 1 + size;
		caret.y += 1;
	}
	auto push_palt = palt; palt = pallette;
	set_color(pallette, 4, index);
	image(gres(COLGRAD), size, ImagePallette);
	palt = push_palt;
}

static void color_picker(int index) {
	auto color_index = color_indecies[index];
	button(INVBUT2, 2, 3);
	if(color_index == -1) {
		button_executed = false;
		return;
	}
	button_colorgrad(color_index, 0);
	fire(buttonparam, color_index, 0, 0);
}

static void color_picker_line(int index, int count, int dx) {
	auto push_caret = caret;
	for(auto i = 0; i < count; i++) {
		color_picker(index++);
		caret.x += dx;
	}
	caret = push_caret;
}

void paint_color_pick() {
	paint_dialog(COLOR);
	setdialog(23, 23, 158, 21); texta(getnm("Colors"), AlignCenter);
	setdialog(21, 51); color_picker_line(0, 6, 28);
	setdialog(21, 79); color_picker_line(6, 6, 28);
	setdialog(21, 107); color_picker_line(12, 6, 28);
	setdialog(21, 135); color_picker_line(18, 6, 28);
	setdialog(21, 163); color_picker_line(24, 6, 28);
	setdialog(49, 191); color_picker_line(30, 4, 28);
	setdialog(24, 220); button(GBTNMED, 1, 2, 0, "DefaultColor"); fire(buttonparam, -1);
	input_cancel(-2);
}

unsigned char open_color_pick(unsigned char current_color, unsigned char default_color) {
	auto result = open_dialog(paint_color_pick, true);
	if(result == -1)
		return default_color;
	else if(result == -2)
		return current_color;
	else
		return (unsigned char)result;
}