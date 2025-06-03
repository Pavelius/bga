#include "array.h"
#include "colorgrad.h"
#include "creature.h"
#include "draw.h"
#include "pushvalue.h"
#include "resid.h"
#include "screenshoot.h"
#include "timer.h"
#include "view.h"

using namespace draw;

extern array console_data;

static point dialog_start;
static bool button_pressed, button_executed, button_hilited, input_disabled;
static bool game_pause;

static void update_frames() {
	update_tick();
	update_game_tick();
}

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

void hotkey(unsigned key, fnevent proc, int param) {
	if(hot.key == key)
		execute(proc, param);
}

void button_check(unsigned key) {
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

void fire(fnevent proc, long param, long param2, const void* object) {
	if(button_executed)
		execute(proc, param, param2, object);
}

void button(resn res, unsigned short f1, unsigned short f2, unsigned key) {
	auto p = gres(res);
	auto& f = p->get(f1);
	width = f.sx; height = f.sy;
	button_check(key);
	image(p, button_pressed ? f2 : f1, 0);
}

void button(resn res, unsigned short f1, unsigned short f2, unsigned key, const char* id) {
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
	pushrect push;
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

static void scroll(resn res, int fu, int fd, int bar) {

}

static void paint_console() {
	static int console_cash_origin, cash_string;
	static size_t cash_size;
	if(!console_data.data)
		return;
	if(console_data.count != cash_size) {
		console_cash_origin = -1;
		console_data.count = cash_size;
	}
	pushfore push({200, 200, 200});
	textf((char*)console_data.data, console_cash_origin, cash_string);
}

static void hilight_protrait() {
	auto push_fore = fore;
	fore = colors::green;
	strokeout(rectb, -1);
	fore = push_fore;
}

static void hilight_drag_protrait() {
	auto push_fore = fore;
	fore = colors::red;
	strokeout(rectb, -1);
	fore = push_fore;
}

static void portrait_small(creature* pc) {
	pushrect push;
	if(selected_creatures.have(pc))
		hilight_protrait();
	setoffset(2, 2);
	image(gres(PORTS), pc->portrait, 0);
	if(drag_item_source && player != pc && ishilite()) {
		drag_item_dest = pc->wears;
		hilight_drag_protrait();
	}
}

static void choose_creature() {
	player = (creature*)hot.object;
	if(!hot.param)
		selected_creatures.clear();
	selected_creatures.add(player);
	invalidate_description();
}

static void hits_bar(int current, int maximum) {
	if(!maximum)
		return;
	auto nw = 45 * current / maximum;
	if(!nw)
		return;
	auto index = 4;
	if(current == maximum)
		index = 0;
	else if(current >= maximum * 4 / 5)
		index = 1;
	else if(current >= maximum * 3 / 5)
		index = 2;
	else if(current >= maximum * 2 / 5)
		index = 3;
	auto push_clipping = clipping;
	setclip({caret.x, caret.y, caret.x + nw, caret.y + height});
	image(gres(GUIHITPT), index, 0);
	clipping = push_clipping;
}

static void creature_hits(const creature* pc) {
	auto push_caret = caret;
	caret.x += 1; caret.y += 48;
	hits_bar(pc->hp, pc->hp_max);
	caret = push_caret;
}

static void portrait_bar() {
	pushrect push;
	caret.x += 505; caret.y += 4;
	width = height = 46;
	for(auto i = 0; i < 6; i++) {
		portrait_small(party[i]);
		creature_hits(party[i]);
		auto key = hot.key & CommandMask;
		if(ishilite() && key == MouseLeft && hot.pressed)
			execute(choose_creature, (hot.key & Shift) != 0, 0, party[i]);
		caret.x += 49;
	}
}

void paint_action_panel() {
	image(gres(GACTN), 1, 0);
	portrait_bar();
}

static void paint_color_pick() {
	paint_dialog(COLOR);
	setdialog(23, 23, 158, 21); texta(getnm("Colors"), AlignCenter);
	setdialog(21, 51); color_picker_line(0, 6, 28);
	setdialog(21, 79); color_picker_line(6, 6, 28);
	setdialog(21, 107); color_picker_line(12, 6, 28);
	setdialog(21, 135); color_picker_line(18, 6, 28);
	setdialog(21, 163); color_picker_line(24, 6, 28);
	setdialog(49, 191); color_picker_line(30, 4, 28);
	setdialog(24, 220); button(GBTNMED, 1, 2, 0, "DefaultColor"); fire(buttonparam, -1);
	hotkey(KeyEscape, buttonparam, -2);
}

static void paint_game_panel() {
	// update_frames();
	dialog_start = caret; image(gres(GCOMM), 0, 0);
	setdialog(736, 43); image(gres(CGEAR), (current_game_tick / 128) % 32, 0); // Rolling world
	setdialog(8, 8, 534, 92); paint_console();
	setdialog(554, 4, 12, 95); scroll(GCOMMSB, 0, 2, 4);
	setdialog(600, 22); button(GCOMMBTN, 4, 5, 'C');
	setdialog(630, 17); button(GCOMMBTN, 6, 7, 'I');
	setdialog(668, 21); button(GCOMMBTN, 8, 9, 'S');
	setdialog(600, 57); button(GCOMMBTN, 14, 15, 'M');
	setdialog(628, 60); button(GCOMMBTN, 12, 13, 'J');
	setdialog(670, 57); button(GCOMMBTN, 10, 11, KeyEscape);
	setdialog(576, 3); button(GCOMMBTN, 0, 1, '*');
	setdialog(703, 2); button(GCOMMBTN, 2, 3);
	setdialog(575, 72); button(GCOMMBTN, 16, 17);
	setdialog(757, 1); button(GCOMMBTN, 18, 19);
	hotkey('Z', change_zoom_factor);
	// HotKey NONE 0 0 0 0 data(QuickSave) hotkey("Ctrl+Q")
	// HotKey NONE 0 0 0 0 data(DebugTest) hotkey("Ctrl+D")
	// HotKey NONE 0 0 0 0 data(ItemList) hotkey("Ctrl+I")
}

void paint_game() {
	if(!game_pause)
		update_frames();
	setcaret(0, 433); paint_action_panel();
	setcaret(0, 493); paint_game_panel();
}

void open_game() {
	scene(paint_game);
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