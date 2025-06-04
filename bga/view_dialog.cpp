#include "array.h"
#include "colorgrad.h"
#include "creature.h"
#include "draw.h"
#include "pushvalue.h"
#include "resid.h"
#include "screenshoot.h"
#include "timer.h"
#include "view.h"
#include "view_list.h"

using namespace draw;

extern array console_data;

static point dialog_start;
static bool button_pressed, button_executed, button_hilited, input_disabled;
static bool game_pause;

static char description_text[4096];
static size_t description_cash_size;
stringbuilder description(description_text);

static void textah(const char* string, unsigned flags) {
	pushfont push(metrics::h1);
	texta(string, flags);
}

static void update_frames() {
	update_tick();
	update_game_tick();
}

static void update_description(const char* format) {
	description.clear();
	description.add(format);
	description_cash_size = -1;
}

static void setdialog(int x, int y) {
	caret = dialog_start + point{(short)x, (short)y};
}

static void setdialog(int x, int y, int w, int h) {
	setdialog(x, y);
	width = w;
	height = h;
}

void paint_dialog(resn v) {
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

static void scroll(resn res, int fu, int fd, int bar, int& origin, int maximum, int per_page, int per_row) {
	if(!maximum)
		return;
	auto pr = gres(res);
	auto& f = pr->get(fu);
	auto w = f.sx;
	auto h = f.sy;
	auto sh = pr->get(bar).sy;
	pushrect push;
	button(res, fu, fu + 1);
	fire(cbsetint, origin - per_row, 0, &origin);
	caret.y = push.caret.y + push.height - h;
	button(res, fd, fd + 1);
	fire(cbsetint, origin + per_row, 0, &origin);
	auto height_max = push.height - h * 2 - sh * 2;
	auto current_position = origin * height_max / maximum;
	caret.y = push.caret.y + h + current_position;
	button(res, bar, bar);
	//button_run_input();
}

static void paint_console() {
	static int origin, maximum;
	static int console_cash_origin, cash_string, cash_origin;
	static size_t cash_size;
	if(!console_data.data)
		return;
	pushrect push;
	const int per_row = texth();
	const int per_page = height;
	if(console_data.count != cash_size) {
		pushrect push;
		cash_string = -1;
		cash_size = console_data.count;
		textfs((char*)console_data.data);
		maximum = height;
	}
	correct_table(origin, maximum, per_page);
	if(cash_origin != origin) {
		cash_origin = origin;
		cash_string = -1;
	}
	pushfore push_fore({200, 200, 200});
	auto push_clip = clipping; setclipall();
	if(cash_string == -1)
		caret.y -= origin;
	textf((char*)console_data.data, console_cash_origin, cash_string);
	clipping = push_clip; caret = push.caret;
	caret.x += width + 16; caret.y -= 4; width = 12; height += 3;
	scroll(GCOMMSB, 0, 2, 4, origin, maximum, per_page, per_row);
}

static void paint_description() {
	static int cash_origin, cash_string;
	if(description.size() != description_cash_size) {
		cash_string = -1;
		description_cash_size = description.size();
	}
	pushfore push({240, 240, 240});
	textf(description, cash_origin, cash_string);
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
	dialog_start = caret; image(gres(GCOMM), 0, 0);
	setdialog(736, 43); image(gres(CGEAR), (current_game_tick / 128) % 32, 0); // Rolling world
	setdialog(12, 8, 526, 92); paint_console();
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
	// HotKey NONE 0 0 0 0 data(DebugTest) hotkey("Ctrl+D")
	// HotKey NONE 0 0 0 0 data(ItemList) hotkey("Ctrl+I")
}

void paint_game() {
	if(!game_pause)
		update_frames();
	setcaret(0, 0, 800, 433); paint_area();
	setcaret(0, 433); paint_action_panel();
	setcaret(0, 493); paint_game_panel();
}

static void paint_item_avatar() {
	auto i = last_item->geti().avatar * 2;
	image(caret.x + width / 2, caret.y + height / 2, gres(ITEMS), i + 1, 0);
}

static void identify_item() {
	last_item->identify(1);
	update_description("%ItemInformation");
}

static void paint_item_description() {
	paint_dialog(GIITMH08);
	setdialog(36, 37, 357, 30); textah(str("%ItemName"), AlignCenterCenter);
	setdialog(430, 20, 64, 64); paint_item_avatar();
	setdialog(20, 432); button(GBTNMED, 1, 2, 'I', "Identify"); fire(identify_item);
	setdialog(179, 432); button(GBTNMED, 1, 2, 'U', "UseItem");
	setdialog(338, 432); button(GBTNMED, 1, 2, KeyEscape, "Done"); fire(buttoncancel);
	setdialog(28, 115, 435, 299); paint_description();
	//TextDescription NORMAL 23 111 445 307 fore(250 250 250) id("Text")
	//Scroll GBTNSCRL 480 109 12 311 frames(1 0 3 2 4 5)
	// Label NORMAL 314 111 152 18 id("Test")
}

void open_item_description() {
	auto push_last = last_item;
	last_item = (item*)hot.object;
	update_description("%ItemInformation");
	open_dialog(paint_item_description, true);
	last_item = push_last;
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