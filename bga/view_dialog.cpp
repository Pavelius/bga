#include "action.h"
#include "array.h"
#include "area.h"
#include "ambient.h"
#include "audio.h"
#include "colorgrad.h"
#include "console.h"
#include "creaturea.h"
#include "draw.h"
#include "formation.h"
#include "game.h"
#include "help.h"
#include "itemground.h"
#include "keybind.h"
#include "option.h"
#include "pushvalue.h"
#include "race.h"
#include "resinfo.h"
#include "screenshoot.h"
#include "timer.h"
#include "list.h"
#include "math.h"
#include "variant.h"
#include "vector.h"
#include "view.h"
#include "view_list.h"

using namespace draw;

extern array console_data;

int game_panel_mode;

int caret_index;
bool button_pressed, button_executed, button_hilited, button_sound, input_disabled;
bool need_update_items;

static point dialog_start;
static bool game_pause;
static fnevent game_proc;
static fnoperation drag_drop_proc;
static item *drag_item_source, *drag_item_dest;
static item drag_item, drag_ground_item;
static char edit_field[32];

sprite* pma_butstd;
sprite* pma_butlrg2;
sprite* pma_butopt1;
sprite* pma_cursors;
sprite* pma_cursarw;
sprite* pma_ground;
sprite* pma_port[2];
sprite* pma_pfcm;
sprite* pma_stoneslot;

static sprite* pma_actn;
static sprite* pma_butact;
static sprite* pma_butinv2;
static sprite* pma_colgrad;
static sprite* pma_form;
static sprite* pma_items;
static sprite* pma_hitp;
static sprite* pma_number;
static sprite* pma_spells;
static sprite* pma_scrsb;

static char description_text[4096];
static int description_cash_size;
static int character_info_mode;
static int current_topic_list, cash_topic_list, current_content_list;
static int current_spell_level;
static vector<nameable*> content;
static vector<spelli*> spells;
static vector<item*> items;
static stringbuilder description(description_text);
static char* input_string;
static int input_string_size;

static void paint_game_inventory();

static adat<spellbook*, 16> spellbooks;

struct pushdescription {
	char value[lenghtof(description_text)];
	pushdescription() { memcpy(value, description_text, sizeof(value)); description.clear(); }
	~pushdescription() { memcpy(description_text, value, sizeof(value)); }
};

static int compare_nameable_ptr(const void* v1, const void* v2) {
	auto p1 = *((nameable**)v1);
	auto p2 = *((nameable**)v2);
	return szcmp(p1->getname(), p2->getname());
}

static void set_invalidate() {
	description_cash_size = -1;
}

void cbsetintds() {
	auto p = (int*)hot.object;
	*p = hot.param;
	set_invalidate();
}

static void set_cursor() {
	cursor.set(pma_cursors, 0);
}

static const char* getnms(abilityn v) {
	return getnm(ids(bsdata<abilityi>::elements[v].id, "Short"));
}

void texta(sprite* ps, const char* string, unsigned flags) {
	pushfont push(ps);
	texta(string, flags);
}

static void texta(sprite* ps, color fore, const char* string, unsigned flags) {
	pushfore push(fore);
	texta(ps, string, flags);
}

static void update_actor_animations() {
	auto push_player = player;
	for(auto& e : bsdata<creature>()) {
		if(!e.ispresent())
			continue;
		player = &e;
		e.updateanimate();
	}
	player = push_player;
}

void update_area_music() {
	auto pa = get_area();
	if(pa)
		play_music(pa->music);
	else
		play_music("MXMAIN");
}

void update_frames() {
	update_tick();
	update_visibility();
	audio_update_channels();
	update_area_music();
	if(!game_pause) {
		update_game_tick();
		update_actor_animations();
		update_ambients(camera);
	}
}

void set_description(const char* format) {
	description.clear();
	description.add(format);
	description_cash_size = -1;
}

void add_description(const char* format) {
	description.add(format);
	description_cash_size = -1;
}

void set_description(const nameable* object) {
	description.clear();
	description_cash_size = -1;
	for(auto& e : bsdata<varianti>()) {
		if(e.pstatus && e.source->have(object)) {
			e.pstatus(object, description);
			return;
		}
	}
	auto pd = getnme(ids(object->id, "Info"));
	if(pd)
		description.add(pd);
}

void set_description_id(const char* id) {
	auto pd = getnme(ids(id, "Info"));
	if(pd)
		set_description(pd);
}

void set_description() {
	set_description_id(((nameable*)hot.object)->id);
}

void setdialog(int x, int y) {
	caret = dialog_start + point{(short)x, (short)y};
}

void setdialog(int x, int y, int w, int h) {
	setdialog(x, y);
	width = w;
	height = h;
}

void paint_dialog(int x, int y, fnevent proc) {
	auto push_dialog = dialog_start;
	dialog_start.x = x;
	dialog_start.y = y;
	proc();
	dialog_start = push_dialog;
}

static void select_spellbooks() {
	spellbooks.clear();
	auto i = player->getindex();
	for(auto& e : bsdata<spellbook>()) {
		if(e.owner != i)
			continue;
		spellbooks.add(&e);
	}
}

static void select_spells() {
	spells.clear();
	auto level = current_spell_level + 1;
	for(auto& e : bsdata<spelli>()) {
		if(!last_spellbook->is(e.getindex()))
			continue;
		if(e.classes[last_spellbook->type] != level)
			continue;
		spells.add(&e);
	}
	spells.sort(compare_nameable_ptr);
}

static void setgameproc(fnevent v, bool cancel_mode) {
	if(game_proc == v || (cancel_mode && game_proc))
		game_proc = 0;
	else
		game_proc = v;
	set_invalidate();
}

static void setgameproc() {
	auto p = (fnevent)hot.object;
	auto m = (bool)hot.param;
	if(p == paint_game_inventory)
		need_update_items = true;
	setgameproc(p, m);
}

static void quit_game() {
	// Before quit game
}

void next_scene() {
	next_scene((fnevent)hot.object);
}

void open_scene() {
	scene((fnevent)hot.object);
}

void paint_dialog(const char* id, int frame) {
	update_tick();
	set_cursor();
	auto p = gres(id);
	auto& f = p->get(frame);
	dialog_start.x = (getwidth() - f.sx) / 2;
	dialog_start.y = (getheight() - f.sy) / 2 - 128;
	if(dialog_start.y < 64)
		dialog_start.y = 64;
	caret = dialog_start;
	image(p, frame, 0);
}

void paint_game_dialog(const char* id, int frame) {
	update_tick();
	audio_update_channels();
	update_area_music();
	set_cursor();
	dialog_start.x = 0;
	dialog_start.y = 0;
	caret = dialog_start;
	image(gres(id), frame, 0);
}

void paint_game_dialog(int x, int y, const char* id, int frame) {
	dialog_start.x = x;
	dialog_start.y = y;
	caret = dialog_start;
	image(gres(id), frame, 0);
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
	button_sound = false;
	if(input_disabled) {
		button_hilited = false;
		return;
	}
	if(button_hilited) {
		if(hot.pressed)
			button_pressed = true;
		if(hot.key == MouseLeft && hot.pressed)
			button_sound = true;
		if(!hot.pressed && hot.key == MouseLeft)
			button_executed = true;
	}
	if(key && hot.key == key) {
		button_sound = true;
		button_rect = rc;
	}
	if(hot.key == InputKeyUp) {
		if(button_rect == rc) {
			button_executed = true;
			button_rect.clear();
		}
	}
	if(button_rect == rc)
		button_pressed = true;
}

static void button_check_sound(resn res) {
	if(!button_sound)
		return;
	play_sound("GAM_09");
}

void fire(fnevent proc, long param, long param2, void* object) {
	if(button_executed)
		execute(proc, param, param2, object);
}

void fire(fnevent proc, long param, long param2, fnevent object) {
	if(button_executed)
		execute(proc, param, param2, (void*)object);
}

static void tips(const char* p) {
	if(button_hilited) {
		if(!tips_sb)
			tips_sb.add(getnm(p));
	}
}

static void tips(const nameable& e) {
	tips(e.id);
}

void button(sprite* p, unsigned short f1, unsigned short f2, unsigned key) {
	auto& f = p->get(f1);
	width = f.sx; height = f.sy;
	button_check(key);
	button_check_sound((resn)100);
	image(p, button_pressed ? f2 : f1, 0);
}

void stoneslot(unsigned short f1, unsigned short f2, unsigned key) {
	button(pma_stoneslot, f1, f2, key);
}

static void button(actionn id) {
	auto& ei = bsdata<actioni>::elements[id];
	button(pma_butact, ei.avatar, ei.avatar + 1, 0);
	tips(ei);
	caret.x += width + 4;
}

static void button(const item& e) {
	auto push = caret;
	button(pma_butact, 100, 101, 0);
	if(button_pressed) {
		caret.x += 1;
		caret.y += 1;
	}
	if(e)
		paint_item(&e);
	caret = push;
	caret.x += width + 4;
}

static void buttona(sprite* ps, int f1, bool checked) {
	auto push_caret = caret;
	button(pma_butact, checked ? 111 : 108, 109, 0);
	if(button_pressed) {
		caret.x += 2;
		caret.y += 2;
	}
	caret.x += 3; caret.y += 3;
	image(ps, f1, 0);
	caret = push_caret;
	caret.x += width + 4;
}

static void button(formationn id, formationn& value) {
	buttona(pma_form, id, id == value);
	fire(cbsetchr, id, 0, &value);
}

static point get_pressed_offset(sprite* pr) {
	if(pr == pma_butlrg2)
		return {1, 2};
	return {1, 1};
}

void button(sprite* pr, unsigned short f1, unsigned short f2, unsigned key, const char* id, bool need_getname) {
	auto push_caret = caret;
	button(pr, f1, f2, key);
	if(button_pressed)
		caret = caret + get_pressed_offset(pr);
	auto push_height = height;
	height -= 2;
	if(need_getname)
		id = str(getnm(id));
	texta(id, AlignCenterCenter);
	height = push_height;
	caret = push_caret;
}

static color get_disable_color() {
	return color(90, 97, 83);
}

void button(sprite* pr, unsigned short f1, unsigned short f2, unsigned key, const char* id, unsigned short fd, bool allowed, bool need_getname) {
	if(allowed)
		button(pr, f1, f2, key, id, need_getname);
	else {
		pushfore push_fore(fore.mix(get_disable_color(), 128));
		auto push_input = input_disabled; input_disabled = true;
		button(pr, fd, f2, key, id, need_getname);
		input_disabled = push_input;
	}
}

void checkbox(int& source, int value, sprite* pres, unsigned short f1, unsigned short f2, unsigned short fc, unsigned key) {
	if(source == value)
		f1 = fc;
	button(pres, f1, f2, key);
	fire(cbsetintds, value, 0, &source);
}

static void button_colorgrad(int index, int size) {
	pushrect push;
	if(hot.pressed && button_hilited) {
		caret.x += 1 + size;
		caret.y += 1;
	}
	auto push_palt = palt; palt = pallette;
	set_color(pallette, 4, index);
	image(pma_colgrad, size, ImagePallette);
	palt = push_palt;
}

static void color_picker(int index) {
	auto color_index = color_indecies[index];
	button(pma_butinv2, 2, 3);
	if(color_index == -1) {
		button_executed = false;
		return;
	}
	button_colorgrad(color_index, 0);
}

static void choose_creature_color() {
	auto pi = bsdata<portraiti>::elements + player->portrait;
	auto index = hot.param;
	switch(index) {
	case 0: set_color("SkinNormal"); break;
	case 1: set_color("HairNormal"); break;
	default: set_color("HairNormal"); break;
	}
	player->colors[index] = open_color_pick(player->colors[index], pi->colors[index]);
}

void creature_color(int index) {
	auto color_index = player->colors[index];
	button(pma_butinv2, 0, 1);
	if(color_index == -1) {
		button_executed = false;
		return;
	}
	button_colorgrad(color_index, 1);
	fire(choose_creature_color, index);
}

static void color_picker_line(int index, int count, int dx) {
	auto push_caret = caret;
	for(auto i = 0; i < count; i++) {
		color_picker(index);
		fire(buttonparam, color_indecies[index]);
		caret.x += dx;
		index++;
	}
	caret = push_caret;
}

void edit(char* string, int maximum, unsigned text_flags, bool upper_case) {
	pushrect push;
	int lenght = zlen(string);
	if(caret_index > lenght)
		caret_index = lenght;
	texta(string, text_flags);
	if(((current_tick / 100) % 10) < 4) {
		auto push_caret = caret;
		auto tw = textw(string);
		auto x = aligned(caret.x, width, text_flags, tw);
		caret.x = x + textw(string, caret_index) + 1;
		line(caret.x, caret.y + texth() - 2);
		caret = push_caret;
	}
	switch(hot.key) {
	case InputSymbol:
		if(caret_index < (maximum - 1) && hot.param >= 0x20) {
			if(upper_case)
				string[caret_index++] = upper_symbol((char)hot.param);
			else
				string[caret_index++] = (char)hot.param;
			string[caret_index] = 0;
		}
		break;
	case KeyBackspace:
		if(caret_index > 0)
			string[--caret_index] = 0;
		break;
	}
}

void edit_number() {
	if(hot.key == InputSymbol) {
		if(hot.param == '+' || hot.param == '-')
			hot.key = 0;
	}
	edit(edit_field, lenghtof(edit_field) - 1, AlignRight, false);
}

static void scroll(sprite* pr, int fu, int fd, int bar, int& origin, int maximum, int per_page, int per_row) {
	if(!maximum)
		return;
	auto& f = pr->get(fu);
	// auto w = f.sx;
	auto h = f.sy;
	auto sh = pr->get(bar).sy;
	pushrect push;
	button(pr, fu, fu + 1);
	fire(cbsetint, origin - per_row, 0, &origin);
	caret.y = push.caret.y + push.height - h;
	button(pr, fd, fd + 1);
	fire(cbsetint, origin + per_row, 0, &origin);
	if(maximum > per_page) {
		auto height_max = push.height - h * 2 - sh * 2;
		auto current_position = origin * height_max / (maximum - per_page);
		caret.y = push.caret.y + h + current_position;
		button(pr, bar, bar);
	}
}

static bool dragging(fnevent paint) {
	pushrect push;
	hot.pressed = false;
	while(ismodal()) {
		drag_drop_proc = 0;
		paint();
		switch(hot.key) {
		case MouseLeft:
			if(!hot.pressed)
				execute(buttonok);
			break;
		case MouseRight:
			if(!hot.pressed)
				execute(buttoncancel);
			break;
		case KeyEscape:
			execute(buttoncancel);
			break;
		}
		domodal();
	}
	return getresult();
}

static void paint_game_panel() {
	paint_game_panel(true, false);
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
		origin = maximum - per_page;
	}
	correct_table(origin, maximum, per_page);
	input_mouse_table(origin, maximum, per_page, per_row);
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
	scroll(pma_scrsb, 0, 2, 4, origin, maximum, per_page, per_row);
}

void paint_description(int scr_x, int scr_y, int scr_height) {
	static int cash_origin, cash_string, origin, maximum;
	pushrect push;
	const int per_row = texth();
	const int per_page = height;
	if(description.size() != description_cash_size) {
		cash_string = -1;
		description_cash_size = description.size();
		pushrect push;
		textfs((char*)description);
		maximum = height;
	}
	input_mouse_table(origin, maximum, per_page, per_row);
	correct_table(origin, maximum, per_page);
	if(cash_origin != origin) {
		cash_origin = origin;
		cash_string = -1;
	}
	pushfore push_fore({240, 240, 240});
	auto push_clip = clipping; setclipall();
	if(cash_string == -1)
		caret.y -= origin;
	textf(description, cash_origin, cash_string);
	clipping = push_clip; caret = push.caret;
	caret.x += push.width + scr_x;
	caret.y += scr_y;
	width = 12; height += scr_height;
	scroll(gres("GBTNSCRL"), 0, 2, 4, origin, maximum, per_page, per_row);
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

static void portrait_large() {
	image(pma_port[1], player->portrait, 0);
}

void get_player_portrait(surface& sm, int index) {
	auto push_canvas = canvas;
	surface sa(42, 42, 32); canvas = &sa;
	image(0, 0, pma_port[0], party[index]->portrait, 0);
	blit(sm, 0, 0, sm.width, sm.height, 0, *canvas, 0, 0, canvas->width, canvas->height);
	canvas = push_canvas;
}

void portrait_small(creature* p, bool player_hilite) {
	pushrect push;
	if(!input_disabled) {
		if(player_hilite) {
			if(p == player)
				hilight_protrait();
		} else if(p->isselected())
			hilight_protrait();
	}
	setoffset(2, 2);
	image(pma_port[0], p->portrait, 0);
	if(ishilite()) {
		tips_sb.add(p->name);
		if(drag_item_source && player != p) {
			drag_item_dest = p->wears;
			hilight_drag_protrait();
		}
	}
}

static void paint_item_avatar() {
	auto i = last_item->geti().avatar * 2;
	image(caret.x + width / 2, caret.y + height / 2, pma_items, i + 1, 0);
}

void choose_creature() {
	player = (creature*)hot.object;
	if(!hot.param)
		clear_selection();
	player->select();
	set_invalidate();
	need_update_items = true;
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
	image(pma_hitp, index, 0);
	clipping = push_clipping;
}

static void creature_hits(const creature* pc) {
	auto push_caret = caret;
	caret.x += 1; caret.y += 48;
	hits_bar(pc->hp, pc->hp_max);
	caret = push_caret;
}

static void portrait_bar(bool player_hilite, bool allow_choose_player) {
	pushrect push;
	caret.x += 505; caret.y += 4;
	width = height = 46;
	for(auto i = 0; i < 6; i++) {
		portrait_small(party[i], player_hilite);
		creature_hits(party[i]);
		if(allow_choose_player) {
			auto key = hot.key & CommandMask;
			if(ishilite() && key == MouseLeft && hot.pressed)
				execute(choose_creature, (hot.key & Shift) != 0, 0, party[i]);
		}
		caret.x += 49;
	}
}

static void paint_player_actions() {
	pushrect push;
	image(pma_actn, 0, 0);
	caret.x += 6; caret.y += 12;
	if(have_multiselect()) {
		button(ActionDefend);
		button(ActionAttack);
		button(ActionStop);
		for(auto i = FormationT; i <= FormationProtect; i = formationn(i + 1))
			button(i, current_formation);
	} else if(player) {
		button(ActionDefend);
		button(player->getweapon());
		button(player->getoffhand());
		button(ActionSpecialAbility);
		button(ActionCast);
	}
}

static void paint_action_panel() {
	if(input_disabled)
		image(pma_actn, 1, 0);
	else
		paint_player_actions();
	portrait_bar(false, !input_disabled);
}

void paint_action_panel_player() {
	setcaret(0, 433);
	image(pma_actn, 1, 0);
	portrait_bar(true, !input_disabled);
}

void paint_action_panel_combat() {
	setcaret(0, 433);
	image(pma_actn, 1, 0);
	portrait_bar(true, false);
}

void paint_action_panel_na() {
	auto push_input = input_disabled; input_disabled = true;
	setcaret(0, 433);
	paint_action_panel();
	input_disabled = push_input;
}

void layer(color v, unsigned char a = 32) {
	auto push_alpha = alpha; alpha = a;
	auto push_fore = fore; fore = v;
	rectf();
	alpha = push_alpha;
	fore = push_fore;
}

static void paint_number(int v, unsigned flags) {
	unsigned char result[16];
	auto m = 0;
	if(!v)
		result[0] = 0;
	else {
		while(v) {
			auto n = v % 10;
			result[m++] = n;
			v /= 10;
		}
	}
	auto w = 0;
	auto h = 0;
	for(auto i = m - 1; i >= 0; i--) {
		auto& f = pma_number->get(result[i]);
		w += f.sx;
		if(h < f.sy)
			h = f.sy;
	}
	auto push_caret = caret;
	caret.x = aligned(caret.x, width, flags, w);
	if((flags & (AlignLeftCenter | AlignRightCenter | AlignCenterCenter)) != 0)
		caret.y += (height - h) / 2;
	else if((flags & (AlignLeftBottom | AlignRightBottom | AlignCenterBottom)) != 0)
		caret.y += (height - h);
	for(auto i = m - 1; i >= 0; i--) {
		auto& f = pma_number->get(result[i]);
		image(pma_number, result[i], 0);
		caret.x += f.sx;
	}
	caret = push_caret;
}

void paint_item(const item* pi, int current_count, int choose_count) {
	if(!pi)
		return;
	pushrect push;
	setoffset(2, 2);
	width = 32;
	height = 32;
	if(pi->needidentify())
		layer(colors::blue);
	if(!player->isusable(*pi))
		layer(colors::red);
	image(pma_items, pi->geti().avatar * 2, 0);
	if(button_hilited && hot.key == MouseRight && !hot.pressed)
		execute(open_item_description, 0, 0, pi);
	if(current_count)
		paint_number(current_count, AlignRightBottom);
	if(choose_count)
		paint_number(choose_count, AlignLeft);
}

void paint_item(const item* pi) {
	paint_item(pi, (pi->count > 1) ? pi->count : 0, 0);
}

static void set_drag_item_cursor() {
	cursor.res = pma_items;
	cursor.cicle = drag_item.geti().avatar * 2 + 1;
}

static void paint_inventory_dragging() {
	drag_item_dest = 0;
	update_frames();
	setcaret(0, 0, 800, 433);
	paint_game_inventory();
	paint_action_panel_player();
	set_drag_item_cursor();
}

static bool allow_use(const item& di, const item& v) {
	if(!v)
		return true;
	auto p = get_creature(&di);
	if(!p)
		return true;
	auto slot = p->getslot(&di);
	if(get_slot(slot) != Backpack) {
		if(!p->isusable(v))
			return false;
	}
	return v.is(slot);
}

static bool drag_drop_item() {
	if(!allow_use(*drag_item_dest, drag_item))
		return false;
	if(!allow_use(*drag_item_source, *drag_item_dest))
		return false;
	auto p1 = get_creature(drag_item_source);
	auto p2 = get_creature(drag_item_dest);
	*drag_item_source = *drag_item_dest;
	*drag_item_dest = drag_item;
	if(p1)
		p1->update();
	if(p2)
		p2->update();
	if(drag_ground_item) {
		add_item(current_area, player->position, drag_ground_item);
		drag_ground_item.clear();
	}
	need_update_items = true;
	return true;
}

static bool drag_drop_equip_item() {
	drag_item_dest = player->wears + drag_item.geti().wear;
	return drag_drop_item();
}

static void begin_drag_item() {
	drag_item_source = (item*)hot.object;
	drag_item = *drag_item_source;
	drag_item_source->clear();
	//if(dragging(paint_inventory_dragging)
	//	&& drag_item_dest
	//	&& allow_use(*drag_item_dest, drag_item)
	//	&& allow_use(*drag_item_source, *drag_item_dest)) {
	//	auto p1 = get_creature(drag_item_source);
	//	auto p2 = get_creature(drag_item_dest);
	//	*drag_item_source = *drag_item_dest;
	//	*drag_item_dest = drag_item;
	//	p1->update();
	//	p2->update();
	//} else if(drag_item_source)
	//	*drag_item_source = drag_item;
	if(dragging(paint_inventory_dragging)
		&& drag_drop_proc
		&& drag_drop_proc()) {
		// All correct
	} else if(drag_item_source)
		*drag_item_source = drag_item;
	drag_item_source = 0;
	drag_item_dest = 0;
	drag_item.clear();
}

static void paint_drag_target(item* pi, wearn slot) {
	if(drag_item_source) {
		if(button_hilited) {
			drag_item_dest = pi;
			drag_drop_proc = drag_drop_item;
			image(pma_stoneslot, 25, 0);
		} else {
			auto p = get_creature(pi);
			if(p) {
				if(slot != Backpack && p->isusable(drag_item)) {
					if(drag_item.is(slot))
						image(pma_stoneslot, 16, 0);
				}
			}
		}
	}
}

static void ground_stoneslot(int f) {
	stoneslot(f, f);
	paint_drag_target(&drag_ground_item, Backpack);
}

static void paint_item_dragable(item* pi) {
	paint_item(pi);
	if(button_hilited) {
		if(!drag_item_source) {
			if(hot.key == MouseLeft && hot.pressed)
				execute(begin_drag_item, 0, 0, pi);
		}
	}
}

static void inventory(wearn slot, int index, int empthy_frame, bool show_back = true) {
	auto pi = player->wears + slot + index;
	if(show_back) {
		auto index_frame = index % 8;
		if(index_frame >= 4)
			index_frame += 4;
		image(pma_stoneslot, index_frame, 0);
	}
	button_check(0);
	paint_drag_target(pi, slot);
	if(*pi)
		paint_item_dragable(pi);
	else if(empthy_frame != -1) {
		if(empthy_frame >= 100) // Item frame
			image(caret.x + 2, caret.y + 2, pma_items, empthy_frame - 100, 0);
		else
			image(caret.x + 2, caret.y + 2, gres("STON"), empthy_frame, 0);
	}
}

static void inventory(wearn slot, int index) {
	inventory(slot, index, -1, true);
}

static void inventory_line(int index) {
	auto push = caret;
	auto index_end = index + 8;
	while(index < index_end) {
		inventory(Backpack, index++, -1, true);
		caret.x += 38;
	}
	caret = push;
}

static void paint_game_player() {
	setdialog(20, 79, 210, 330); portrait_large();
	setdialog(22, 23, 206, 28); texta(metrics::h2, player->getname(), AlignCenterCenter);
	paint_action_panel_player();
}

static void apply_weight_color() {
	switch(player->getencumbrance()) {
	case 3: case 2: fore = colors::red; break;
	case 1: fore = colors::yellow; break;
	default: break;
	}
}

static void paint_weight() {
	pushfore push_fore;
	char temp[260]; stringbuilder sb(temp);
	sb.add(getkg(player->weight));
	sb.adds("%-1", getnm("From"));
	sb.adds(getkg(player->allowed_weight));
	apply_weight_color();
	texta(metrics::small, temp, AlignCenterCenter);
}

static void quick_weapon(int index) {
	pushrect push;
	auto fb = index * 3;
	auto pb1 = gres("INVBUT3");
	if(player->weapon_index == index)
		button(pb1, fb + 2, fb + 1, '1' + index);
	else
		button(pb1, fb + 0, fb + 1, '1' + index);
	fire(cbsetchr, index, 0, &player->weapon_index);
	caret.x += 28;
	if(player->weapon_index == index) {
		inventory(QuickWeapon, index * 2, 100);
		image(pma_stoneslot, 34, 0);
	} else
		inventory(QuickWeapon, index * 2, 17);
	caret.x += 38;
	inventory(QuickOffhand, index * 2, 13);
	if(player->weapon_index == index && player->useoffhand())
		image(pma_stoneslot, 34, 0);
}

void paperdoll() {
	static int orientation = 1;
	auto push_caret = caret;
	caret.x += width / 2;
	caret.y += height / 2 + 20;
	paperdoll(*player, player->resid, 1, orientation, current_tick / 100);
	caret = push_caret;
	switch(hot.key) {
	case KeyLeft:
		execute(cbsetint, (orientation >= 15) ? 0 : orientation + 1, 0, &orientation);
		break;
	case KeyRight:
		execute(cbsetint, (orientation <= 0) ? 15 : orientation - 1, 0, &orientation);
		break;
	}
}

static void paperdoll_dragable() {
	button_check(0);
	paperdoll();
	if(drag_item_source) {
		if(button_hilited)
			drag_drop_proc = drag_drop_equip_item;
	}
}

static void update_items() {
	if(!need_update_items)
		return;
	need_update_items = false;
	items.clear();
	auto position = player->position;
	for(auto& e : bsdata<itemground>()) {
		if(e.area == current_area && e && e.position == position)
			items.add(&e);
	}
}

static void view_gorund_item(void* object) {
	auto p = (item*)object;
	if(*p)
		paint_item_dragable(p);
}

static void view_ground_items() {
	static int origin;
	paint_list(items.data, 0, items.count, origin, 3, 2,
		view_gorund_item, 40, 40, {5, 3}, -7, 0, 0);
}

static void paint_game_inventory() {
	update_items();
	paint_game_dialog("GUIINV");
	paint_game_player();
	setdialog(339, 86, 126, 160); paperdoll_dragable();
	setdialog(251, 299, 36, 36); inventory_line(0);
	setdialog(251, 339, 36, 36); inventory_line(8);
	setdialog(251, 379, 36, 36); inventory_line(16);
	setdialog(383, 22, 36, 36); inventory(Head, 0, 0, false);
	setdialog(446, 22, 36, 36); inventory(Neck, 0, 1, false);
	setdialog(255, 22, 36, 36); inventory(Body, 0, 2, false);
	setdialog(319, 22, 36, 36); inventory(Rear, 0, 3, false);
	setdialog(255, 79, 36, 36); inventory(LeftFinger, 0, 4, false);
	setdialog(510, 79, 36, 36); inventory(RightFinger, 0, 5, false);
	setdialog(255, 136, 36, 36); inventory(Hands, 0, 6, false);
	setdialog(510, 22, 36, 36); inventory(Gridle, 0, 7, false);
	setdialog(510, 136, 36, 36); inventory(Legs, 0, 8, false);
	setdialog(574, 130, 111, 22); texta(getnm("Quiver"), AlignCenterCenter);
	setdialog(572, 158, 36, 36); inventory(Quiver, 0);
	setdialog(611, 158, 36, 36); inventory(Quiver, 1);
	setdialog(650, 158, 36, 36); inventory(Quiver, 2);
	setdialog(574, 200, 111, 22); texta(getnm("QuickItem"), AlignCenterCenter);
	setdialog(572, 228, 36, 36); inventory(QuickItem, 0);
	setdialog(611, 228, 36, 36); inventory(QuickItem, 1);
	setdialog(650, 228, 36, 36); inventory(QuickItem, 2);
	setdialog(574, 270, 111, 22); texta(getnm("Ground"), AlignCenterCenter);
	setdialog(572, 299, 36, 36); ground_stoneslot(0);
	setdialog(572, 339, 36, 36); ground_stoneslot(1);
	setdialog(572, 379, 36, 36); ground_stoneslot(2);
	setdialog(612, 299, 36, 36); ground_stoneslot(3);
	setdialog(612, 339, 36, 36); ground_stoneslot(0);
	setdialog(612, 379, 36, 36); ground_stoneslot(1);
	setdialog(570, 298, 80, 120); view_ground_items();
	setdialog(575, 20, 206, 22); texta(getnm("QuickWeapon"), AlignCenterCenter);
	setdialog(572, 48); quick_weapon(0);
	setdialog(572, 88); quick_weapon(1);
	setdialog(679, 48); quick_weapon(2);
	setdialog(679, 88); quick_weapon(3);
	setdialog(704, 141, 70, 20); texta(str("%1i", player->coins), AlignCenterCenter);
	setdialog(704, 243, 70, 32); texta(metrics::h2, str("%1i", player->get(AC)), AlignCenterCenter);
	setdialog(710, 353, 54, 16); texta(metrics::h2, str("%1i", player->hp_max), AlignCenterCenter);
	setdialog(710, 371, 54, 16); texta(metrics::h2, str("%1i", player->hp), AlignCenterCenter);
	setdialog(252, 191, 42, 42); creature_color(HairColor); tips("HairColor");
	setdialog(252, 231, 42, 42); creature_color(SkinColor); tips("SkinColor");
	setdialog(507, 191, 42, 42); creature_color(MajorColor); tips("MajorColor");
	setdialog(507, 231, 42, 42); creature_color(MinorColor); tips("MinorColor");
	setdialog(341, 281, 117, 14); paint_weight();
}

static void paint_color_pick() {
	paint_dialog("COLOR");
	setdialog(23, 23, 158, 21); texta(getnm("Colors"), AlignCenter);
	setdialog(21, 51); color_picker_line(0, 6, 28);
	setdialog(21, 79); color_picker_line(6, 6, 28);
	setdialog(21, 107); color_picker_line(12, 6, 28);
	setdialog(21, 135); color_picker_line(18, 6, 28);
	setdialog(21, 163); color_picker_line(24, 6, 28);
	setdialog(49, 191); color_picker_line(30, 4, 28);
	setdialog(24, 220); button(gres("GBTNMED"), 1, 2, 0, "DefaultColor"); fire(buttonparam, -1);
	hotkey(KeyEscape, buttonparam, -2);
}

static void quit_main_menu() {
	if(!confirm("ConfirmQuitMainMenu"))
		return;
}

static void toggle_option_flag() {
	auto n = (optionf)hot.param;
	optflags.set(n, !getvalue(n));
	hot.object = bsdata<optionfi>::elements + n;
	set_description();
}

static void toggle_option_value(optionv n, int v) {
	optvalues[n] = v;
	hot.object = bsdata<optionvi>::elements + n;
	set_description();
}

//static void toggle_option_value() {
//	toggle_option_value((optionv)hot.param, hot.param2);
//}

static void checkbox(optionf id) {
	texta(bsdata<optionfi>::elements[id].getname(), AlignRightCenter);
	button_check(0);
	fire(set_description, 0, 0, bsdata<optionfi>::elements + id);
	caret.x += width + 12; caret.y -= 3;
	button(gres("GBTNOPT3"), getvalue(id) ? 3 : 0, 1); fire(toggle_option_flag, id);
}

static void slider(optionv id, int slider_width = 142) {
	auto& ei = bsdata<optionvi>::elements[id];
	texta(ei.getname(), AlignRightCenter);
	button_check(0);
	fire(set_description, 0, 0, &ei);
	auto value = getvalue(id) - ei.minimal;
	auto range = ei.maximum - ei.minimal;
	auto maximum_width = slider_width - 24;
	if(!range || maximum_width <= 0)
		return;
	caret.x += width + 24; width = maximum_width;
	auto position = value * maximum_width / range;
	button_hilited = ishilite();
	button_pressed = button_hilited && hot.pressed;
	button_executed = (hot.key == MouseLeft && !hot.pressed);
	image(caret.x + position, caret.y + 2, gres("GUISLDR"), button_pressed ? 1 : 0, 0);
	if(button_pressed)
		toggle_option_value(id, ei.minimal + (hot.mouse.x - caret.x - 8) * range / maximum_width);
}

static void paint_game_opt_pause() {
	paint_game_dialog("GOPT", 3);
	setdialog(279, 23, 242, 30); texta(metrics::h1, getnm("AutoPause"), AlignCenterCenter);
	setdialog(74, 70, 308, 18); checkbox(PauseCharacterHit);
	setdialog(74, 97, 308, 18); checkbox(PauseCharacterInjured);
	setdialog(74, 124, 308, 18); checkbox(PauseCharacterDeath);
	setdialog(74, 151, 308, 18); checkbox(PauseCharacterAttacked);
	setdialog(74, 178, 308, 18); checkbox(PauseWeaponUnusable);
	setdialog(74, 205, 308, 18); checkbox(PauseEnemyDestroyed);
	setdialog(74, 232, 308, 18); checkbox(PauseEndOfRound);
	setdialog(74, 259, 308, 18); checkbox(PauseEnemySighted);
	setdialog(74, 286, 308, 18); checkbox(PauseSpellCast);
	setdialog(74, 313, 308, 18); checkbox(PauseTrapDetected);
	setdialog(74, 340, 308, 18); checkbox(PauseCharacterCenter);
	setdialog(438, 71, 270, 253); paint_description(9, -2, 4);
	setdialog(614, 338); button(gres("GBTNSTD"), 1, 2, KeyEscape, "Done"); fire(buttonok);
}

static void open_game_opt_pause() {
	pushdescription push;
	open_dialog(paint_game_opt_pause, false);
}

static void paint_game_opt_game_play() {
	paint_game_dialog("GOPT", 2);
	setdialog(279, 23, 242, 30); texta(metrics::h1, getnm("GamePlay"), AlignCenterCenter);
	setdialog(74, 70, 184, 18); slider(ToolTipsDelay);
	setdialog(74, 99, 184, 18); slider(MouseScrollSpeed);
	setdialog(74, 128, 184, 18); slider(KeyboardScrollSpeed);
	setdialog(74, 157, 184, 18); slider(GameDifficult);
	setdialog(74, 187, 308, 18); checkbox(DitherAlways);
	setdialog(74, 307, 308, 18); checkbox(ShowGore);
	setdialog(74, 217, 308, 18); checkbox(GroupDarkvision);
	setdialog(74, 247, 308, 18); checkbox(ShowWeather);
	setdialog(74, 277, 308, 18); checkbox(MaximumHitPointsPerLevel);
	setdialog(438, 71, 270, 253); paint_description(9, -2, 4);
	setdialog(71, 338); button(pma_butstd, 1, 2);
	setdialog(194, 338); button(pma_butstd, 1, 2, 'P', "AutoPause"); fire(open_game_opt_pause);
	// setdialog(491, 338); button(GBTNSTD, 1, 2);
	setdialog(614, 338); button(pma_butstd, 1, 2, KeyEscape, "Done"); fire(buttonok);
}

static void open_game_opt_game_play() {
	pushdescription push;
	open_dialog(paint_game_opt_game_play, true);
}

static void confirm_quit_game() {
	if(!confirm("ConfirmQuitGame"))
		return;
	next_scene(quit_game);
}

static void paint_game_options() {
	auto pb1 = gres("GBTNLRG2");
	paint_game_dialog("STONEOPT");
	paint_action_panel_na();
	setdialog(279, 23, 242, 30); texta(metrics::h1, getnm("Options"), AlignCenterCenter);
	setdialog(497, 68); button(pb1, 1, 2, '1', "LoadGame"); fire(open_load_game);
	setdialog(497, 98); button(pb1, 1, 2, '2', "SaveGame"); fire(open_save_game);
	setdialog(497, 128); button(pb1, 1, 2, '3', "QuitMainMenu"); fire(quit_main_menu);
	setdialog(497, 168); button(pb1, 1, 2, '4', "Graphics");
	setdialog(497, 198); button(pb1, 1, 2, '5', "Sound");
	setdialog(497, 228); button(pb1, 1, 2, '6', "GamePlay"); fire(open_game_opt_game_play);
	setdialog(497, 268); button(pb1, 1, 2, '7', "Movies");
	setdialog(497, 298); button(pb1, 1, 2, '8', "Keyboard");
	setdialog(555, 338); button(pma_butstd, 1, 2, 0, "Close"); fire(setgameproc, 1, 0, (void*)paint_game_options);
	setdialog(353, 386, 95, 16); texta(str("%GameVersion"), AlignCenterCenter);
}

static void paint_game_journal() {
	auto pb1 = gres("GBTNJBTN");
	paint_game_dialog("GUIJRNL");
	setdialog(234, 24, 205, 28); texta(metrics::h1, getnm("Journal"), AlignCenterCenter);
	setdialog(66, 90, 651, 275); texta("Test text 1", AlignLeft); // fore(255 255 246)
	//Scroll GBTNSCRL 727 64 12 304 frames(1 0 3 2 4 5)
	setdialog(460, 18); button(pb1, 1, 2);
	setdialog(525, 18); button(pb1, 5, 6);
	setdialog(66, 67, 170, 20); texta("Test text 2", AlignLeft); // fore(0 200 200)
	paint_action_panel_na();
}

static void ability(abilityn v) {
	pushfore push_fore;
	texta(getnms(v), AlignCenterCenter);
	caret.x += 51; width = 32;
	texta(str("%1i", player->abilities[v]), AlignCenterCenter);
	caret.x += 41;
	auto n = player->getbonus(v);
	if(n > 0)
		fore = colors::green;
	else if(n < 0)
		fore = colors::red;
	texta(str("%+1i", n), AlignCenterCenter);
}

void paint_list(void* source, int size, int& origin, int& current, int maximum, int per_page, fngetname get_name, unsigned flags) {
	pushrect push;
	pushfore push_fore;
	input_mouse_table(origin, maximum, per_page, 1);
	caret.y += 1;
	height = texth() + 2;
	correct_table(origin, maximum, per_page);
	if(maximum > origin + per_page)
		maximum = origin + per_page;
	for(auto i = origin; i < maximum; i++) {
		auto p = (char*)source + size * i;
		fore = (current == i) ? colors::yellow : colors::white;
		text(get_name(p));
		button_hilited = ishilite();
		if(button_hilited) {
			if(hot.key == MouseLeft && !hot.pressed)
				execute(cbsetint, i, 0, &current);
		}
		caret.y += height;
	}
}

static void paint_list(const array& source, int& origin, int& current, int per_page) {
	pushrect push;
	pushfore push_fore;
	auto push_clip = clipping; setclipall();
	int maximum = source.count;
	input_mouse_table(origin, maximum, per_page, 1);
	caret.y += 1;
	height = texth() + 2;
	correct_table(origin, maximum, per_page);
	auto im = maximum;
	if(im > origin + per_page)
		im = origin + per_page;
	for(auto i = origin; i < im; i++) {
		auto p = ((nameable**)source.data)[i];
		fore = (current == i) ? colors::yellow : colors::white;
		text(p->getname());
		button_hilited = ishilite();
		if(button_hilited) {
			if(hot.key == MouseLeft && !hot.pressed)
				execute(cbsetint, i, 0, &current);
		}
		caret.y += height;
	}
	clipping = push_clip;
	caret.x += push.width + 13;
	caret.y = push.caret.y - 2;
	height = push.height + 5; width = 12;
	scroll(gres("GBTNSCRL"), 0, 2, 4, origin, maximum, per_page, 1);
}

void paint_list(void* data, int size, int maximum, int& origin, int per_page, fncommand proc, int row_height, point scr, int scr_height, fnevent action_proc, fnevent info_proc) {
	pushrect push;
	pushfore push_fore;
	auto push_clip = clipping; setclipall();
	input_mouse_table(origin, maximum, per_page, 1);
	height = row_height;
	correct_table(origin, maximum, per_page);
	auto im = maximum;
	if(im > origin + per_page)
		im = origin + per_page;
	auto ref_list = false;
	auto push_dialog = dialog_start;
	if(!size) {
		size = sizeof(void*);
		ref_list = true;
	}
	for(list_row_index = origin; list_row_index < im; list_row_index++) {
		button_hilited = ishilite();
		auto p = (char*)data + list_row_index * size;
		if(ref_list)
			p = *((char**)p);
		dialog_start = caret;
		proc(p);
		if(button_hilited) {
			if(hot.key == MouseLeft && !hot.pressed) {
				if(action_proc)
					execute(action_proc, list_row_index, 0, p);
			} else if(hot.key == MouseRight && !hot.pressed) {
				if(info_proc)
					execute(info_proc, list_row_index, 0, p);
			}
		}
		caret.y += row_height;
	}
	dialog_start = push_dialog;
	clipping = push_clip;
	caret.x += push.width;
	caret.y = push.caret.y;
	caret = caret + scr;
	height = push.height + scr_height; width = 12;
	scroll(gres("GBTNSCRL"), 0, 2, 4, origin, maximum, per_page, 1);
}

void paint_list(void* data, int size, int maximum, int& origin, int row_count, int per_row, fncommand proc, int row_width, int row_height, point scr, int scr_height, fnevent action_proc, fnevent info_proc) {
	pushrect push;
	pushfore push_fore;
	auto push_clip = clipping; setclipall();
	auto per_page = row_count * per_row;
	input_mouse_table(origin, maximum, per_page, per_row);
	height = row_height; width = row_width;
	correct_table(origin, maximum, per_page);
	auto im = maximum;
	if(im > origin + per_page)
		im = origin + per_page;
	auto ref_list = false;
	auto push_dialog = dialog_start;
	if(!size) {
		size = sizeof(void*);
		ref_list = true;
	}
	auto push_caret = caret;
	for(list_row_index = origin; list_row_index < im; list_row_index++) {
		auto n = list_row_index - origin;
		button_hilited = ishilite();
		auto p = (char*)data + list_row_index * size;
		if(ref_list)
			p = *((char**)p);
		dialog_start = caret;
		proc(p);
		if(button_hilited) {
			if(hot.key == MouseLeft && !hot.pressed) {
				if(action_proc)
					execute(action_proc, list_row_index, 0, p);
			} else if(hot.key == MouseRight && !hot.pressed) {
				if(info_proc)
					execute(info_proc, list_row_index, 0, p);
			}
		}
		if((n % per_row) == (per_row - 1)) {
			caret.x = push_caret.x;
			caret.y += row_height;
		} else
			caret.x += row_width;
	}
	caret = push_caret;
	dialog_start = push_dialog;
	clipping = push_clip;
	caret.x += push.width;
	caret.y = push.caret.y;
	caret = caret + scr;
	height = push.height + scr_height; width = 12;
	scroll(gres("GBTNSCRL"), 0, 2, 4, origin, maximum, row_count * per_row, per_row);
}

static void paint_topic_lists() {
	static int origin;
	paint_list(bsdata<helpi>::elements, sizeof(bsdata<helpi>::elements[0]),
		origin, current_topic_list, bsdata<helpi>::source.count, 15, nameable::getname, AlignLeftCenter);
}

static void select_content(int index) {
	if(cash_topic_list == index)
		return;
	cash_topic_list = index;
	content.clear();
	auto& ei = bsdata<helpi>::elements[index];
	auto pb = ei.source.begin();
	auto pe = ei.source.end();
	auto sz = ei.source.element_size;
	for(auto ps = pb; ps < pe; ps += sz) {
		if(ei.filter) {
			if(!ei.filter(ps))
				continue;
		}
		content.add((nameable*)ps);
	}
	content.sort(compare_nameable);
}

static void paint_content_lists() {
	static int origin;
	select_content(current_topic_list);
	paint_list(content, origin, current_content_list, 15);
}

static void paint_help() {
	paint_game_dialog("GUIHELP");
	// paint_action_panel_na();
	// paint_game_panel_na();
	setdialog(300, 23, 200, 30); texta(metrics::h1, getnm("Information"), AlignCenterCenter);
	setdialog(297, 373); button(gres("GBTNBFRM"), 1, 2, KeyEscape, "Close"); fire(buttoncancel);
	setdialog(74, 72, 95, 286); paint_topic_lists();
	setdialog(194, 72, 197, 286); paint_content_lists();
	description.clear();
	if(content && content.count > (size_t)current_content_list) {
		auto p = content[current_content_list];
		if(p)
			set_description(p);
	}
	setdialog(435, 72, 271, 286); paint_description(14, -2, 5);
}

static void open_help() {
	cash_topic_list = -1;
	open_dialog(paint_help, true);
}

static void update_character() {
	if(description_cash_size == -1) {
		switch(character_info_mode) {
		case 0: set_description("%PlayerInformation"); break;
		case 1: set_description("%PlayerCombatInformation"); break;
		case 2: set_description("%PlayerSkillInformation"); break;
		}
	}
}

static void paint_game_character() {
	auto pb1 = gres("GBTNRECB");
	paint_game_dialog("GUIREC");
	paint_game_player();
	setdialog(258, 23, 115, 28); texta(bsdata<racei>::elements[player->race].getname(), AlignCenterCenter);
	setdialog(253, 78, 45, 30); ability(Strenght);
	setdialog(253, 116, 45, 30); ability(Dexterity);
	setdialog(253, 155, 45, 30); ability(Constitution);
	setdialog(253, 193, 45, 30); ability(Intelligence);
	setdialog(253, 233, 45, 30); ability(Wisdow);
	setdialog(253, 271, 45, 30); ability(Charisma);
	setdialog(463, 381, 32, 30); texta(str("%1i", player->get(AC)), AlignCenterCenter);
	setdialog(585, 378, 54, 16); texta(str("%1i", player->hp_max), AlignCenterCenter);
	setdialog(585, 399, 54, 16); texta(str("%1i", player->hp), AlignCenterCenter);
	setdialog(256, 307); button(pma_butstd, 1, 2, 0, "Information"); fire(open_help);
	setdialog(256, 334); button(pma_butstd, 1, 2, 0, "Biography");
	setdialog(256, 361); button(pma_butstd, 1, 2, 0, "Export");
	setdialog(256, 388); button(pma_butstd, 1, 2, 0, "Customize");
	setdialog(430, 22); checkbox(character_info_mode, 0, pb1, 0, 1, 2, 0);
	setdialog(512, 22); checkbox(character_info_mode, 1, pb1, 3, 4, 5, 0);
	setdialog(594, 22); checkbox(character_info_mode, 2, pb1, 6, 7, 8, 0);
	setdialog(676, 22); checkbox(character_info_mode, 3, pb1, 9, 10, 11, 0);
	setdialog(655, 379); button(pma_butstd, 1, 2, 'L', "LevelUp");
	update_character();
	setdialog(406, 64, 349, 288); paint_description(13, 0, 6);
	//UpdateCreatureInfo NONE 0 0 0 0
	//Scroll GBTNSCRL 768 64 12 294 frames(1 0 3 2 4 5)
}

static void paint_worldmap() {
	paint_game_dialog("GUIMAP", 1);
	setdialog(666, 18, 113, 22); texta(getnm("WorldMap"), AlignCenterCenter);
	setdialog(680, 288); button(gres("GUIMAPWC"), 0, 1, 'W'); fire(next_scene, 0, 0, (void*)open_game);
	setdialog(23, 20, 630, 392); paint_worldmap_area();
}

static void paint_game_automap() {
	paint_game_dialog("GUIMAP");
	paint_action_panel_na();
	setdialog(696, 56, 82, 20); texta(getnm("AreaNotes"), AlignCenterCenter);
	setdialog(680, 288); button(gres("GUIMAPWC"), 0, 1, 'W'); fire(next_scene, 0, 0, (void*)open_worldmap);
	setdialog(664, 54); button(gres("GBTNOPT1"), 1, 2);
	setdialog(666, 18, 113, 22); texta(getnm("AreaMap"), AlignCenterCenter);
	setdialog(98, 36, 480, 360); paint_minimap();
	setdialog(668, 92, 109, 165); // Map notes text
}

static void paint_spell_description() {
	auto pb1 = gres("GBTNMED");
	paint_dialog("GIITMH08");
	setdialog(36, 37, 357, 30); texta(metrics::h1, last_spell->getname(), AlignCenterCenter);
	setdialog(446, 36); image(pma_spells, last_spell->avatar, 0);
	setdialog(338, 432); button(pb1, 1, 2, KeyEscape, "Done"); fire(buttoncancel);
	setdialog(28, 115, 435, 299); paint_description(17, -6, 12);
}

//static void paint_spell_description_old() {
//	paint_dialog("GUISPL", 2);
//	setdialog(22, 22, 343, 20); texta(getnm("Spell"), AlignCenterCenter);
//	setdialog(22, 52, 343, 20); texta(metrics::font, colors::yellow, last_spell->getname(), AlignCenterCenter);
//	setdialog(27, 87, 355, 304); paint_description(14, -5, 9);
//	setdialog(375, 22); image(pma_spells, last_spell->avatar, 0);
//	//Scroll GBTNSCRL 396 82 12 313 frames(1 0 3 2 4 5)
//	setdialog(135, 402); button(gres("GBTNMED"), 1, 2, KeyEscape, "Done"); fire(buttoncancel);
//}

static void open_spell_info() {
	last_spell = (spelli*)hot.object;
	play_sound("GAM_03");
	set_description("%SpellInformation");
	open_dialog(paint_spell_description, true);
}

static void spell_level_filter() {
	setdialog(740, 62, 42, 38);
	auto pb2 = gres("GBTNSPB2");
	for(auto i = 0; i < 9; i++) {
		auto frame = i * 3 + 1;
		if(current_spell_level == i)
			frame = i * 3 + 0;
		button(pb2, frame, i * 3 + 2, 0);
		fire(cbsetint, i, 0, &current_spell_level);
		caret.y += 39;
	}
}

static void spell_type_filter() {
	static int origin, current;
	const auto per_page = 4;
	int maximum = spellbooks.count;
	auto pb1 = gres("GBTNSPB3");
	auto pb2 = gres("GBTNSPB1");
	correct_table(origin, maximum, per_page);
	setdialog(252, 19); button(pb1, 0, 1); fire(cbsetint, origin - 1, 0, &origin);
	setdialog(705, 19); button(pb1, 2, 3); fire(cbsetint, origin + 1, 0, &origin);
	setdialog(273, 19);
	if(maximum > origin + per_page)
		maximum = origin + per_page;
	auto index = origin;
	for(; index < maximum; index++) {
		auto frame = 1;
		if(index == current)
			frame = 0;
		button(pb2, frame, 2, 0, spellbooks[index]->getid());
		fire(cbsetint, index, 0, &current);
		caret.x += 108;
	}
	for(; index < 4; index++) {
		button(pb2, 3, 3);
		caret.x += 108;
	}
	last_spellbook = spellbooks[current];
}

static void paint_spell(void* object) {
	pushrect push;
	auto p = (spelli*)object;
	image(pma_spells, p->avatar, 0);
	caret.x += 32; width -= 32; height -= 12;
	texta(p->getname(), AlignCenterCenter);
}

static int spell_slot_used() {
	auto r = 0;
	for(auto p : spells)
		r += last_spellbook->powers[p->getindex()];
	return r;
}

static int spell_slot_maximum() {
	return player->getspellslots((classn)(last_spellbook->type), current_spell_level + 1);
}

static void spell_action() {
	auto p = (spelli*)hot.object;
	auto m = spell_slot_maximum();
	auto u = spell_slot_used();
	if(u < m)
		last_spellbook->powers[p->getindex()]++;
}

static void paint_spell_memorized() {
	adat<spelli*, 32> source;
	for(auto p : spells) {
		if(last_spellbook->powers[p->getindex()])
			source.add(p);
	}
	pushrect push;
	width = 32; height = 32;
	for(auto y = 0; y < 8; y++) {
		for(auto x = 0; x < 3; x++) {
			size_t index = y * 3 + x;
			if(index >= source.count)
				return;
			caret.x = push.caret.x + x * 79;
			caret.y = push.caret.y + y * 39;
			auto p = source.data[index];
			auto pi = last_spellbook->powers + p->getindex();
			texta(str("%1i", *pi), AlignCenterCenter);
			image(caret.x + 39, caret.y, pma_spells, p->avatar, 0);
			if(hot.mouse.in({caret.x, caret.y, caret.x + 79, caret.y + height})) {
				if(hot.key == MouseLeft && !hot.pressed)
					execute(cbsetchr, *pi - 1, 0, pi);
				else if(hot.key == MouseLeft && !hot.pressed)
					execute(open_spell_info, 0, 0, pi);
			}
		}
	}
}

static void paint_game_spells() {
	paint_game_dialog("GUISPL");
	paint_game_player();
	setdialog(253, 50, 228, 19); texta(getnm("MemorizedSpells"), AlignCenterCenter);
	setdialog(492, 50, 228, 19); texta(getnm("KnownSpells"), AlignCenterCenter);
	setdialog(253, 391, 187, 19); texta(getnm("SpellSlotsAvailable"), AlignCenterCenter);
	setdialog(742, 20, 39, 38); texta(getnm("SpellLevelShort"), AlignCenterCenter);
	select_spellbooks();
	if(spellbooks) {
		static int origin;
		spell_type_filter();
		spell_level_filter();
		select_spells();
		setdialog(494, 80, 207, 331); paint_list(spells.data, 0, spells.count, origin, 8, paint_spell, 42, {7, -1}, -1, spell_action, open_spell_info);
		setdialog(252, 78); paint_spell_memorized();
		auto m = spell_slot_maximum();
		auto u = spell_slot_used();
		setdialog(449, 390, 32, 20); texta(str("%1i", m - u), AlignCenterCenter);
	} else {
		pushfore push_fore(colors::black);
		auto push_alpha = alpha; alpha = 128;
		setdialog(0, 0, 800, 433); rectf();
		alpha = push_alpha;
	}
}

void paint_game_panel(bool allow_input, bool combat_mode) {
	pushrect push;
	auto push_dialog = dialog_start;
	setcaret(0, 493);
	dialog_start = caret; image(gres("GCOMM"), 0, 0);
	setdialog(12, 8, 526, 92); paint_console();
	auto pb1 = gres("GCOMMBTN");
	auto pb2 = gres("CGEAR");
	if(allow_input) {
		setdialog(736, 43); image(pb2, (current_game_tick / 128) % 32, 0); // Rolling world
		setdialog(600, 22); button(pb1, 4, 5, 'C'); fire(setgameproc, 0, 0, paint_game_character); tips("CharacterSheet");
		setdialog(630, 17); button(pb1, 6, 7, 'I'); fire(setgameproc, 0, 0, paint_game_inventory); tips("CharacterInventory");
		setdialog(668, 21); button(pb1, 8, 9, 'S'); fire(setgameproc, 0, 0, paint_game_spells); tips("Spells");
		setdialog(600, 57); button(pb1, 14, 15, 'M'); fire(setgameproc, 0, 0, paint_game_automap); tips("AreaMap");
		setdialog(628, 60); button(pb1, 12, 13, 'J'); fire(setgameproc, 0, 0, paint_game_journal); tips("Journal");
		setdialog(670, 57); button(pb1, 10, 11, KeyEscape); fire(setgameproc, 1, 0, paint_game_options); tips("Options");
		setdialog(576, 3); button(pb1, 0, 1, '*'); fire(select_all_party); tips("SelectAllParty");
		setdialog(703, 2); button(pb1, 2, 3); tips("RestParty");
		setdialog(575, 72); button(pb1, 16, 17); tips("ModifyParty");
		hotkey('Z', change_zoom_factor);
	} else {
		setdialog(575, 2, 225, 105);
		layer(colors::black, 128);
	}
	if(combat_mode) {
		setdialog(757, 1);
		image(pb1, 19, 0);
		//button(GCOMMBTN, 18, 19);
	}
	dialog_start = push_dialog;
}

void change_panel_mode() {
	game_panel_mode = (++game_panel_mode) % 3;
}

void view_game_area() {
	switch(game_panel_mode) {
	case 0:
		setcaret(0, 0, 800, 433);
		paint_area();
		setcaret(0, 433); paint_action_panel();
		paint_game_panel();
		break;
	case 1:
		setcaret(0, 0, 800, 433 + 107);
		paint_area();
		setcaret(0, 433 + 107); paint_action_panel();
		break;
	default:
		setcaret(0, 0, 800, 600);
		paint_area();
		break;
	}
}

static void view_game() {
	update_frames();
	if(game_proc) {
		setcaret(0, 0, 800, 433);
		game_proc();
		paint_game_panel();
	} else
		view_game_area();
	input_debug();
}

static void set_item_description() {
	set_description("%ItemInformation");
}

static void identify_item() {
	last_item->identify(1);
	set_item_description();
}

static void paint_item_description() {
	auto pb1 = gres("GBTNMED");
	paint_dialog("GIITMH08");
	setdialog(36, 37, 357, 30); texta(metrics::h1, str("%ItemName"), AlignCenterCenter);
	setdialog(430, 20, 64, 64); paint_item_avatar();
	if(last_item->needidentify()) {
		setdialog(20, 432); button(pb1, 1, 2, 'I', "Identify"); fire(identify_item);
	}
	if(last_item->canuse()) {
		setdialog(179, 432); button(pb1, 1, 2, 'U', "UseItem");
	}
	setdialog(338, 432); button(pb1, 1, 2, KeyEscape, "Done"); fire(buttoncancel);
	setdialog(28, 115, 435, 299); paint_description(17, -6, 12);
}

static void add_edit() {
	auto r = 0; psnum(edit_field, r);
	auto n = r + hot.param;
	if(n < 0)
		n = 0;
	stringbuilder sb(edit_field); sb.add("%1i", n);
	caret_index = -1;
}

static void view_item_count() {
	paint_dialog("GUIINV", 2);
	setdialog(22, 22); paint_item(last_item, 0, 0);
	setdialog(20, 90); button(pma_butstd, 1, 2, KeyEnter, "Accept"); fire(buttonok);
	setdialog(142, 90); button(pma_butstd, 1, 2, KeyEscape, "Cancel"); fire(buttoncancel);
	setdialog(222, 44); button(gres("GBTNPLUS"), 0, 1, '+'); fire(add_edit, 1);
	setdialog(242, 44); button(gres("GBTNMINS"), 0, 1, '-'); fire(add_edit, -1);
	setdialog(71, 22, 186, 18); texta(getnm("ChooseAmount"), AlignCenterCenter);
	pushfont push_font(metrics::small);
	setdialog(179, 47, 34, 13); edit_number();
}

static void paint_main_menu() {
	auto pb1 = gres("GBTNMED2");
	paint_game_dialog("START", 1);
	setdialog(569, 133, 152, 21); texta(getnm("GameMode"), AlignCenterCenter);
	setdialog(567, 160); button(pb1, 1, 2, 'M', "SinglePlayer");
	setdialog(569, 220, 152, 21); texta(getnm("BeginGame"), AlignCenterCenter);
	setdialog(567, 248); button(pb1, 5, 6, 'N', "NewGame"); fire(open_select_party);
	setdialog(567, 280); button(pb1, 9, 10, 'L', "LoadGame"); fire(open_load_game);
	setdialog(567, 312); button(pb1, 13, 14, 'Q', "QuickLoad"); fire(game_quick_load);
	setdialog(567, 344); button(pb1, 1, 2, 'J', "JoinGame", 3, false);
	setdialog(567, 396); button(pb1, 5, 6, 'O', "Options");
	setdialog(567, 428); button(pb1, 9, 10, KeyEscape, "QuitGame"); fire(confirm_quit_game);
	input_debug();
}

void open_main_menu() {
	// play_list("MAIN", PlayDay);
	scene(paint_main_menu);
}

static void paint_name_dialog() {
	paint_dialog("GUICNAME");
	setdialog(22, 22, 233, 20); texta(getnm("CharacterName"), AlignCenterCenter);
	setdialog(25, 57, 220, 20); edit(input_string, input_string_size, AlignLeftCenter);
	setdialog(19, 84); button(pma_butstd, 1, 2, KeyEscape, "Cancel"); fire(buttoncancel);
	setdialog(141, 84); button(pma_butstd, 1, 2, KeyEnter, "Done", 3, input_string[0] != 0); fire(buttonok);
}

bool open_name(char* result, int size) {
	if(!result)
		return false;
	input_string = result;
	input_string_size = size;
	return open_dialog(paint_name_dialog, true);
}

static bool paint_tips() {
	static point position;
	static unsigned long tips_stamp;
	const char* pn = tips_sb;
	if(!pn || pn[0]==0)
		return false;
	auto ps = gres("TOOLTIP");
	if(!ps)
		return false;
	auto n = getcputime();
	if(hot.pressed)
		tips_stamp = 0;
	if(!tips_stamp)
		tips_stamp = n;
	if(tips_stamp + optvalues[ToolTipsDelay] > n) {
		position = hot.mouse;
		return false;
	}
	if(position && distance(position, hot.mouse) > 4) {
		position.clear();
		tips_stamp = 0;
		return false;
	}
	const int pad_x = 8;
	caret.x = position.x;
	caret.y = position.y + 4;
	width = textw(pn) + pad_x * 2;
	height = 32;
	if(caret.x >= getwidth() - width - 8)
		caret.x = getwidth() - width - 8;
	if(caret.y >= getheight() - height - 8)
		caret.y = getheight() - height - 8;
	image(caret.x, caret.y, ps, 1, ImageNoOffset);
	auto push_clip = clipping; setclip({caret.x + 3, caret.y, caret.x + width - 3, caret.y + height});
	image(caret.x + 3, caret.y, ps, 0, ImageNoOffset);
	clipping = push_clip;
	image(caret.x + width - 3, caret.y, ps, 4, ImageNoOffset);
	caret.y += (30 - texth()) / 2;
	caret.x += pad_x - 2;
	text(pn);
	return true;
}

static void paint_cursor() {
	auto pi = cursor.res;
	if(!pi)
		return;
	auto cicle = cursor.cicle;
	if(cursor.res == pma_cursors) {
		auto pressed = hot.pressed;
		if(pressed)
			cicle += 1;
		image(hot.mouse.x, hot.mouse.y, pi, cicle, 0);
	} else if(cursor.res == pma_cursarw)
		image(hot.mouse.x, hot.mouse.y, pi, pi->ganim(cursor.cicle, current_tick / 32), 0);
	else
		image(hot.mouse.x, hot.mouse.y, pi, cicle, 0);
}

void open_item_description() {
	pushdescription push_info;
	pushvalue push(last_item, (item*)hot.object);
	play_sound("GAM_03");
	set_item_description();
	open_dialog(paint_item_description, true);
}

void open_item_count() {
	last_number = 0;
	if(!last_item)
		return;
	stringbuilder sb(edit_field); sb.add("%1i", last_item->count);
	caret_index = -1;
	open_dialog(view_item_count, true);
	if(!getresult())
		return;
	psnum(edit_field, last_number);
}

void open_game() {
	game_proc = 0;
	clear_path_map();
	scene(view_game);
}

void open_worldmap() {
	scene(paint_worldmap);
}

static void view_confirm() {
	paint_dialog("GUIERR", 1);
	setdialog(28, 28, 221, 64); texta(description, AlignCenterCenter);
	setdialog(18, 104); button(pma_butstd, 1, 2, KeyEnter, "Yes"); fire(buttonok);
	setdialog(140, 104); button(pma_butstd, 1, 2, KeyEscape, "No"); fire(buttoncancel);
}

bool confirm(const char* id, ...) {
	pushdescription push;
	XVA_FORMAT(id);
	description.clear();
	description.addv(getnm(id), format_param);
	open_dialog(view_confirm, true);
	return getresult() != 0;
}

static void view_message() {
	paint_dialog("GUIERR", 1);
	setdialog(28, 28, 221, 64); texta(description, AlignCenterCenter);
	setdialog(64, 104); button(pma_butstd, 1, 2, KeyEnter, "OK"); fire(buttonok);
	hotkey(KeyEscape, buttonok);
}

void warning(const char* id, ...) {
	pushdescription push;
	XVA_FORMAT(id);
	description.clear();
	description.addv(getnm(id), format_param);
	play_sound("GAM_47");
	open_dialog(view_message, true);
}

void statusv(const char* prefix, const char* format, const char* format_param) {
	char temp[260]; stringbuilder sb(temp);
	sb.addv(format, format_param);
	printclf();
	printcnv("[");
	printcnv(prefix);
	printcnv(temp);
	printcnv("]");
}

void statusr(const char* id, ...) {
	XVA_FORMAT(id);
	statusv("-", getnm(id), format_param);
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

static void tips_main() {
	audio_update_channels();
	if(paint_tips())
		return;
	paint_cursor();
}

static void textf_widgets() {
	if(equal(textf_widget, "answers"))
		widget_answers();
}

void initialize_ui() {
	set_cursor();
	ptips = tips_main;
	textf_proc = textf_widgets;
}

void initialize_interface() {
	pma_items = gres("ITEMS");
	pma_spells = gres("SPELLS");
	pma_actn = gres("GACTN");
	pma_cursors = gres("CURSORS");
	pma_cursarw = gres("CURSARW");
	pma_number = gres("NUMBER");
	pma_port[0] = gres("PORTS");
	pma_port[1] = gres("PORTL");
	pma_stoneslot = gres("STONSLOT");
	pma_colgrad = gres("COLGRAD");
	pma_hitp = gres("GUIHITPT");
	pma_pfcm = gres("GUIPFC");
	pma_butopt1 = gres("GBTNOPT1");
	pma_butact = gres("GUIBTACT");
	pma_form = gres("FORM");
	pma_scrsb = gres("GCOMMSB");
	pma_butstd = gres("GBTNSTD");
	pma_butlrg2 = gres("GBTNLRG2");
	pma_butinv2 = gres("INVBUT2");
	pma_ground = gres("GROUND");
}