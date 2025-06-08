#include "array.h"
#include "colorgrad.h"
#include "creature.h"
#include "draw.h"
#include "help.h"
#include "pushvalue.h"
#include "resid.h"
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

static point dialog_start;
static bool button_pressed, button_executed, button_hilited, input_disabled;
static bool game_pause;
static fnevent game_proc;
static fnoperation drag_drop_proc;
static item *drag_item_source, *drag_item_dest;
static item drag_item;

static char description_text[4096];
static size_t description_cash_size;
static int character_info_mode;
static int current_topic_list, cash_topic_list, current_content_list;
static int current_spell_level;
static vector<nameable*> content;
static vector<spelli*> spells;
static stringbuilder description(description_text);

static void paint_game_inventory();

static adat<spellbook*, 16> spellbooks;

fnevent on_player_change;

struct pushdescription {
	char value[lenghtof(description_text)];
	pushdescription() { memcpy(value, description_text, sizeof(value)); }
	~pushdescription() { memcpy(description_text, value, sizeof(value)); }
};

static int compare_nameable(const void* v1, const void* v2) {
	auto p1 = *((nameable**)v1);
	auto p2 = *((nameable**)v2);
	return szcmp(p1->getname(), p2->getname());
}

static void set_invalidate() {
	description_cash_size = -1;
}

static void cbsetintds() {
	auto p = (int*)hot.object;
	*p = hot.param;
	set_invalidate();
}

static void set_cursor() {
	cursor.set(CURSORS, 0);
}

static const char* getnms(abilityn v) {
	return getnm(ids(bsdata<abilityi>::elements[v].id, "Short"));
}

void texta(resn res, const char* string, unsigned flags) {
	pushfont push(gres(res));
	texta(string, flags);
}

void texta(resn res, color fore, const char* string, unsigned flags) {
	pushfore push(fore);
	texta(res, string, flags);
}

static void update_frames() {
	update_tick();
	if(!game_pause)
		update_game_tick();
}

void set_description(const char* id) {
	description.clear();
	description.add(id);
	description_cash_size = -1;
}

void setdialog(int x, int y) {
	caret = dialog_start + point{(short)x, (short)y};
}

void setdialog(int x, int y, int w, int h) {
	setdialog(x, y);
	width = w;
	height = h;
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
	spells.sort(compare_nameable);
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
	setgameproc(p, m);
}

void next_scene() {
	next_scene((fnevent)hot.object);
}

void open_scene() {
	scene((fnevent)hot.object);
}

void paint_dialog(resn v, int frame) {
	set_cursor();
	auto p = gres(v);
	auto& f = p->get(frame);
	dialog_start.x = (getwidth() - f.sx) / 2;
	dialog_start.y = (getheight() - f.sy) / 2 - 128;
	if(dialog_start.y < 64)
		dialog_start.y = 64;
	caret = dialog_start;
	image(p, frame, 0);
}

void paint_game_dialog(resn v, int frame) {
	set_cursor();
	dialog_start.x = 0;
	dialog_start.y = 0;
	caret = dialog_start;
	image(gres(v), frame, 0);
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

static point get_pressed_offset(resn n) {
	switch(n) {
	case GBTNMED2: return {1, 2};
	default: return {1, 1};
	}
}

void button(resn res, unsigned short f1, unsigned short f2, unsigned key, const char* id) {
	auto push_caret = caret;
	button(res, f1, f2, key);
	if(button_pressed)
		caret = caret + get_pressed_offset(res);
	auto push_height = height;
	height -= 2;
	texta(str(getnm(id)), AlignCenterCenter);
	height = push_height;
	caret = push_caret;
}

static color get_disable_color(resn res) {
	return color(90, 97, 83);
}

void button(resn res, unsigned short f1, unsigned short f2, unsigned key, const char* id, unsigned short fd, bool allowed) {
	if(allowed)
		button(res, f1, f2, key, id);
	else {
		pushfore push_fore(fore.mix(get_disable_color(res), 128));
		auto push_input = input_disabled; input_disabled = true;
		button(res, fd, f2, key, id);
		input_disabled = push_input;
	}
}

void checkbox(int& source, int value, resn res, unsigned short f1, unsigned short f2, unsigned short fc, unsigned key) {
	if(source == value)
		f1 = fc;
	button(res, f1, f2, key);
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

static void creature_color(int index) {
	auto color_index = player->colors[index];
	button(INVBUT2, 0, 1);
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
		fire(buttonparam, color_indecies[index], 0, 0);
		caret.x += dx;
		index++;
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
	if(maximum > per_page) {
		auto height_max = push.height - h * 2 - sh * 2;
		auto current_position = origin * height_max / (maximum - per_page);
		caret.y = push.caret.y + h + current_position;
		button(res, bar, bar);
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
	paint_game_panel(true);
}

static void paint_game_panel_na() {
	paint_game_panel(false);
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
	scroll(GBTNSCRL, 0, 2, 4, origin, maximum, per_page, per_row);
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
	image(gres(PORTL), player->portrait, 0);
}

static void portrait_small(creature* pc, bool player_hilite) {
	pushrect push;
	if(!input_disabled) {
		if(player_hilite) {
			if(pc == player)
				hilight_protrait();
		} else if(pc->isselected())
			hilight_protrait();
	}
	setoffset(2, 2);
	image(gres(PORTS), pc->portrait, 0);
	if(drag_item_source && player != pc && ishilite()) {
		drag_item_dest = pc->wears;
		hilight_drag_protrait();
	}
}

static void paint_item_avatar() {
	auto i = last_item->geti().avatar * 2;
	image(caret.x + width / 2, caret.y + height / 2, gres(ITEMS), i + 1, 0);
}

static void choose_creature() {
	player = (creature*)hot.object;
	if(!hot.param)
		clear_selection();
	player->select();
	set_invalidate();
	if(on_player_change)
		on_player_change();
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

static void portrait_bar(bool player_hilite) {
	pushrect push;
	caret.x += 505; caret.y += 4;
	width = height = 46;
	for(auto i = 0; i < 6; i++) {
		portrait_small(party[i], player_hilite);
		creature_hits(party[i]);
		if(!input_disabled) {
			auto key = hot.key & CommandMask;
			if(ishilite() && key == MouseLeft && hot.pressed)
				execute(choose_creature, (hot.key & Shift) != 0, 0, party[i]);
		}
		caret.x += 49;
	}
}

static void paint_action_panel() {
	setcaret(0, 433);
	image(gres(GACTN), 1, 0);
	portrait_bar(false);
}

void paint_action_panel_player() {
	setcaret(0, 433);
	image(gres(GACTN), 1, 0);
	portrait_bar(true);
}

void paint_action_panel_na() {
	auto push_input = input_disabled; input_disabled = true;
	paint_action_panel();
	input_disabled = push_input;
}

static void layer(color v, unsigned char a = 32) {
	auto push_alpha = alpha; alpha = a;
	auto push_fore = fore; fore = v;
	rectf();
	alpha = push_alpha;
	fore = push_fore;
}

void paint_item(const item* pi) {
	if(!pi)
		return;
	pushrect push;
	setoffset(2, 2);
	if(!player->isusable(*pi))
		layer(colors::red);
	image(gres(ITEMS), pi->geti().avatar * 2, 0);
	if(button_hilited && hot.key == MouseRight && !hot.pressed)
		execute(open_item_description, 0, 0, pi);
}

static void set_drag_item_cursor() {
	cursor.id = ITEMS;
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
	p1->update();
	p2->update();
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
			image(gres(STONSLOT), 25, 0);
		} else {
			auto p = get_creature(pi);
			if(p) {
				if(slot != Backpack && p->isusable(drag_item)) {
					if(drag_item.is(slot))
						image(gres(STONSLOT), 16, 0);
				}
			}
		}
	}
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
		image(gres(STONSLOT), index_frame, 0);
	}
	button_check(0);
	paint_drag_target(pi, slot);
	if(*pi)
		paint_item_dragable(pi);
	else if(empthy_frame != -1) {
		if(empthy_frame >= 100) // Item frame
			image(caret.x + 2, caret.y + 2, gres(ITEMS), empthy_frame - 100, 0);
		else
			image(caret.x + 2, caret.y + 2, gres(STON), empthy_frame, 0);
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
	setdialog(22, 23, 206, 28); texta(REALMS, player->getname(), AlignCenterCenter);
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
	texta(TOOLFONT, temp, AlignCenterCenter);
}

static void quick_weapon(int index) {
	pushrect push;
	auto fb = index * 3;
	if(player->weapon_index == index)
		button(INVBUT3, fb + 2, fb + 1, '1' + index);
	else
		button(INVBUT3, fb + 0, fb + 1, '1' + index);
	fire(cbsetchr, index, 0, &player->weapon_index);
	caret.x += 28;
	if(player->weapon_index == index) {
		inventory(QuickWeapon, index * 2, 100);
		image(gres(STONSLOT), 34, 0);
	} else
		inventory(QuickWeapon, index * 2, 17);
	caret.x += 38;
	inventory(QuickOffhand, index * 2, 13);
	if(player->weapon_index == index && player->useoffhand())
		image(gres(STONSLOT), 34, 0);
}

static void paperdoll_dragable() {
	button_check(0);
	paperdoll();
	if(drag_item_source) {
		if(button_hilited)
			drag_drop_proc = drag_drop_equip_item;
	}
}

static void paint_game_inventory() {
	paint_game_dialog(GUIINV);
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
	setdialog(572, 299, 36, 36); button(STONSLOT, 0, 0, 0);
	setdialog(572, 339, 36, 36); button(STONSLOT, 0, 0, 0);
	setdialog(572, 379, 36, 36); button(STONSLOT, 0, 0, 0);
	setdialog(612, 299, 36, 36); button(STONSLOT, 1, 1, 0);
	setdialog(612, 339, 36, 36); button(STONSLOT, 1, 1, 0);
	setdialog(612, 379, 36, 36); button(STONSLOT, 1, 1, 0);
	//	Scroll GBTNSCRL 655 302 12 112 frames(1 0 3 2 4 5)
	setdialog(575, 20, 206, 22); texta(getnm("QuickWeapon"), AlignCenterCenter);
	setdialog(572, 48); quick_weapon(0);
	setdialog(572, 88); quick_weapon(1);
	setdialog(679, 48); quick_weapon(2);
	setdialog(679, 88); quick_weapon(3);
	setdialog(704, 141, 70, 20); texta(NORMAL, str("%1i", player->coins), AlignCenterCenter);
	setdialog(704, 243, 70, 32); texta(REALMS, str("%1i", player->get(AC)), AlignCenterCenter);
	setdialog(710, 353, 54, 16); texta(REALMS, str("%1i", player->hp_max), AlignCenterCenter);
	setdialog(710, 371, 54, 16); texta(REALMS, str("%1i", player->hp), AlignCenterCenter);
	setdialog(252, 191, 42, 42); creature_color(1);
	setdialog(252, 231, 42, 42); creature_color(0);
	setdialog(507, 191, 42, 42); creature_color(2);
	setdialog(507, 231, 42, 42); creature_color(3);
	setdialog(341, 281, 117, 14); paint_weight();
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

static void paint_game_options() {
	paint_game_dialog(STONEOPT);
	paint_action_panel_na();
	setdialog(279, 23, 242, 30); texta(STONEBIG, getnm("Options"), AlignCenterCenter);
	setdialog(497, 68); button(GBTNLRG2, 1, 2);
	setdialog(497, 98); button(GBTNLRG2, 1, 2);
	setdialog(497, 128); button(GBTNLRG2, 1, 2);
	setdialog(353, 386, 95, 16); texta(getnm("Version"), AlignCenterCenter);
	setdialog(497, 168); button(GBTNLRG2, 1, 2);
	setdialog(497, 198); button(GBTNLRG2, 1, 2);
	setdialog(555, 338); button(GBTNSTD, 1, 2, KeyEscape, "Close"); fire(setgameproc, 1, 0, paint_game_options);
}

static void paint_game_journal() {
	paint_game_dialog(GUIJRLN);
	setdialog(234, 24, 205, 28); texta(STONEBIG, getnm("Journal"), AlignCenterCenter);
	setdialog(66, 90, 651, 275); texta("Test text 1", AlignLeft); // fore(255 255 246)
	//Scroll GBTNSCRL 727 64 12 304 frames(1 0 3 2 4 5)
	setdialog(460, 18); button(GBTNJBTN, 1, 2);
	setdialog(525, 18); button(GBTNJBTN, 5, 6);
	setdialog(66, 67, 170, 20); texta("Test text 2", AlignLeft); // fore(0 200 200)
	paint_action_panel_na();
}

static void ability(abilityn v) {
	pushfore push_fore;
	texta(getnms(v), AlignCenterCenter);
	caret.x += 51; width = 32;
	texta(str("%1i", player->abilitites[v]), AlignCenterCenter);
	caret.x += 41;
	auto n = player->getbonus(v);
	if(n > 0)
		fore = colors::green;
	else if(n < 0)
		fore = colors::red;
	texta(str("%+1i", n), AlignCenterCenter);
}

static void paint_list(void* source, size_t size, int& origin, int& current, int maximum, int per_page, fngetname get_name, unsigned flags) {
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
	scroll(GBTNSCRL, 0, 2, 4, origin, maximum, per_page, 1);
}

static void paint_list(const array& source, int& origin, int per_page, fncommand proc, int row_height, point scr, int scr_height, fnevent action_proc, fnevent info_proc) {
	pushrect push;
	pushfore push_fore;
	auto push_clip = clipping; setclipall();
	int maximum = source.count;
	input_mouse_table(origin, maximum, per_page, 1);
	height = row_height;
	correct_table(origin, maximum, per_page);
	auto im = maximum;
	if(im > origin + per_page)
		im = origin + per_page;
	for(auto i = origin; i < im; i++) {
		auto p = ((void**)source.data)[i];
		proc(p);
		button_hilited = ishilite();
		if(button_hilited) {
			if(hot.key == MouseLeft && !hot.pressed) {
				if(action_proc)
					execute(action_proc, i, 0, p);
			} else if(hot.key == MouseRight && !hot.pressed) {
				if(info_proc)
					execute(info_proc, i, 0, p);
			}
		}
		caret.y += height;
	}
	clipping = push_clip;
	caret.x += push.width;
	caret.y = push.caret.y;
	caret = caret + scr;
	height = push.height + scr_height; width = 12;
	scroll(GBTNSCRL, 0, 2, 4, origin, maximum, per_page, 1);
}

void paint_list(void* data, size_t size, int maximum, int& origin, int per_page, fncommand proc, int row_height, point scr, int scr_height, fnevent action_proc, fnevent info_proc) {
	pushrect push;
	pushfore push_fore;
	auto push_clip = clipping; setclipall();
	input_mouse_table(origin, maximum, per_page, 1);
	height = row_height;
	correct_table(origin, maximum, per_page);
	auto im = maximum;
	if(im > origin + per_page)
		im = origin + per_page;
	for(auto i = origin; i < im; i++) {
		button_hilited = ishilite();
		auto p = (char*)data + i * size;
		proc(p);
		if(button_hilited) {
			if(hot.key == MouseLeft && !hot.pressed) {
				if(action_proc)
					execute(action_proc, i, 0, p);
			} else if(hot.key == MouseRight && !hot.pressed) {
				if(info_proc)
					execute(info_proc, i, 0, p);
			}
		}
		caret.y += height;
	}
	clipping = push_clip;
	caret.x += push.width;
	caret.y = push.caret.y;
	caret = caret + scr;
	height = push.height + scr_height; width = 12;
	scroll(GBTNSCRL, 0, 2, 4, origin, maximum, per_page, 1);
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
	paint_game_dialog(GUIHELP);
	// paint_action_panel_na();
	// paint_game_panel_na();
	setdialog(300, 23, 200, 30); texta(STONEBIG, getnm("Information"), AlignCenterCenter);
	setdialog(297, 373); button(GBTNBFRM, 1, 2, KeyEscape, "Close"); fire(buttoncancel);
	setdialog(74, 72, 95, 286); paint_topic_lists();
	setdialog(194, 72, 197, 286); paint_content_lists();
	description.clear();
	if(content && content.count > (size_t)current_content_list) {
		auto p = content[current_content_list];
		if(p) {
			auto pv = find_variant(p);
			if(pv && pv->pstatus)
				pv->pstatus(p, description);
			else
				description.add(getnme(ids(p->id, "Info")));
		}
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
		case 1: set_description("%PlayerSkillInformation"); break;
		}
	}
}

static void paint_game_character() {
	paint_game_dialog(GUIREC);
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
	setdialog(256, 307); button(GBTNSTD, 1, 2, 0, "Information"); fire(open_help);
	setdialog(256, 334); button(GBTNSTD, 1, 2, 0, "Biography");
	setdialog(256, 361); button(GBTNSTD, 1, 2, 0, "Export");
	setdialog(256, 388); button(GBTNSTD, 1, 2, 0, "Customize");
	setdialog(430, 22); checkbox(character_info_mode, 0, GBTNRECB, 0, 1, 2, 0);
	setdialog(512, 22); checkbox(character_info_mode, 1, GBTNRECB, 3, 4, 5, 0);
	setdialog(594, 22); checkbox(character_info_mode, 2, GBTNRECB, 6, 7, 8, 0);
	setdialog(676, 22); checkbox(character_info_mode, 3, GBTNRECB, 9, 10, 11, 0);
	setdialog(655, 379); button(GBTNSTD, 1, 2, 'L', "LevelUp");
	update_character();
	setdialog(406, 64, 349, 288); paint_description(13, 0, 6);
	//UpdateCreatureInfo NONE 0 0 0 0
	//Scroll GBTNSCRL 768 64 12 294 frames(1 0 3 2 4 5)
}

static void paint_worldmap() {
	paint_game_dialog(GUIMAP, 1);
	setdialog(666, 18, 113, 22); texta(getnm("WorldMap"), AlignCenterCenter);
	setdialog(680, 288); button(GUIMAPWC, 0, 1, 'W'); fire(next_scene, 0, 0, open_game);
	setdialog(23, 20, 630, 392); paint_worldmap_area();
}

static void paint_game_automap() {
	paint_game_dialog(GUIMAP);
	paint_action_panel_na();
	setdialog(696, 56, 82, 20); texta(getnm("AreaNotes"), AlignCenterCenter);
	setdialog(680, 288); button(GUIMAPWC, 0, 1, 'W'); fire(next_scene, 0, 0, open_worldmap);
	setdialog(664, 54); button(GBTNOPT1, 1, 2);
	setdialog(666, 18, 113, 22); texta(getnm("AreaMap"), AlignCenterCenter);
	setdialog(98, 36, 480, 360); paint_minimap();
	setdialog(668, 92, 109, 165); // Map notes text
}

static void paint_spell_description() {
	paint_dialog(GUISPL, 2);
	setdialog(22, 22, 343, 20); texta(getnm("Spell"), AlignCenterCenter);
	setdialog(22, 52, 343, 20); texta(NORMAL, colors::yellow, last_spell->getname(), AlignCenterCenter);
	setdialog(27, 87, 355, 304); paint_description(14, -5, 9);
	setdialog(375, 22); image(gres(SPELLS), last_spell->avatar, 0);
	//Scroll GBTNSCRL 396 82 12 313 frames(1 0 3 2 4 5)
	setdialog(135, 402); button(GBTNMED, 1, 2, KeyEscape, "Done"); fire(buttoncancel);
}

static void open_spell_info() {
	last_spell = (spelli*)hot.object;
	set_description("%SpellInformation");
	open_dialog(paint_spell_description, true);
}

static void spell_level_filter() {
	setdialog(740, 62, 42, 38);
	for(auto i = 0; i < 9; i++) {
		auto frame = i * 3 + 1;
		if(current_spell_level == i)
			frame = i * 3 + 0;
		button(GBTNSPB2, frame, i * 3 + 2, 0);
		fire(cbsetint, i, 0, &current_spell_level);
		caret.y += 39;
	}
}

static void spell_type_filter() {
	static int origin, current;
	const auto per_page = 4;
	int maximum = spellbooks.count;
	correct_table(origin, maximum, per_page);
	setdialog(252, 19); button(GBTNSPB3, 0, 1); fire(cbsetint, origin - 1, 0, &origin);
	setdialog(705, 19); button(GBTNSPB3, 2, 3); fire(cbsetint, origin + 1, 0, &origin);
	setdialog(273, 19);
	if(maximum > origin + per_page)
		maximum = origin + per_page;
	auto index = origin;
	for(; index < maximum; index++) {
		auto frame = 1;
		if(index == current)
			frame = 0;
		button(GBTNSPB1, frame, 2, 0, spellbooks[index]->getid());
		fire(cbsetint, index, 0, &current);
		caret.x += 108;
	}
	for(; index < 4; index++) {
		button(GBTNSPB1, 3, 3);
		caret.x += 108;
	}
	last_spellbook = spellbooks[current];
}

static void paint_spell(void* object) {
	pushrect push;
	auto p = (spelli*)object;
	image(gres(SPELLS), p->avatar, 0);
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
			image(caret.x + 39, caret.y, gres(SPELLS), p->avatar, 0);
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
	paint_game_dialog(GUISPL);
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
		setdialog(494, 80, 207, 331); paint_list(spells, origin, 8, paint_spell, 42, {7, -1}, -1, spell_action, open_spell_info);
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

void paint_game_panel(bool allow_input) {
	pushrect push;
	auto push_dialog = dialog_start;
	setcaret(0, 493);
	dialog_start = caret; image(gres(GCOMM), 0, 0);
	setdialog(12, 8, 526, 92); paint_console();
	if(allow_input) {
		setdialog(736, 43); image(gres(CGEAR), (current_game_tick / 128) % 32, 0); // Rolling world
		setdialog(600, 22); button(GCOMMBTN, 4, 5, 'C'); fire(setgameproc, 0, 0, paint_game_character);
		setdialog(630, 17); button(GCOMMBTN, 6, 7, 'I'); fire(setgameproc, 0, 0, paint_game_inventory);
		setdialog(668, 21); button(GCOMMBTN, 8, 9, 'S'); fire(setgameproc, 0, 0, paint_game_spells);
		setdialog(600, 57); button(GCOMMBTN, 14, 15, 'M'); fire(setgameproc, 0, 0, paint_game_automap);
		setdialog(628, 60); button(GCOMMBTN, 12, 13, 'J'); fire(setgameproc, 0, 0, paint_game_journal);
		setdialog(670, 57); button(GCOMMBTN, 10, 11, KeyEscape); fire(setgameproc, 1, 0, paint_game_options);
		setdialog(576, 3); button(GCOMMBTN, 0, 1, '*'); fire(select_all_party);
		setdialog(703, 2); button(GCOMMBTN, 2, 3);
		setdialog(575, 72); button(GCOMMBTN, 16, 17);
		setdialog(757, 1); button(GCOMMBTN, 18, 19);
		hotkey('Z', change_zoom_factor);
	} else {
		setdialog(575, 2, 225, 105);
		layer(colors::black, 128);
	}
	dialog_start = push_dialog;
}

void paint_game() {
	update_frames();
	setcaret(0, 0, 800, 433);
	if(game_proc)
		game_proc();
	else {
		paint_area();
		paint_action_panel();
	}
	paint_game_panel();
	input_debug();
}

static void identify_item() {
	last_item->identify(1);
	set_description("###%ItemName\n%ItemInformation");
}

static void paint_item_description() {
	paint_dialog(GIITMH08);
	setdialog(36, 37, 357, 30); texta(STONEBIG, getnm("Item"), AlignCenterCenter);
	setdialog(430, 20, 64, 64); paint_item_avatar();
	setdialog(20, 432); button(GBTNMED, 1, 2, 'I', "Identify"); fire(identify_item);
	setdialog(179, 432); button(GBTNMED, 1, 2, 'U', "UseItem");
	setdialog(338, 432); button(GBTNMED, 1, 2, KeyEscape, "Done"); fire(buttoncancel);
	setdialog(28, 115, 435, 299); paint_description(17, -6, 12);
}

static void paint_cursor() {
	auto pi = gres(cursor.id);
	if(!pi)
		return;
	auto cicle = cursor.cicle;
	if(cursor.id == CURSORS) {
		auto pressed = hot.pressed;
		if(pressed)
			cicle += 1;
		image(hot.mouse.x, hot.mouse.y, pi, cicle, 0);
	} else if(cursor.id == CURSARW) {
		auto ti = pi->ganim(cursor.cicle, current_tick / 32);
		image(hot.mouse.x, hot.mouse.y, pi, ti, 0);
	} else
		image(hot.mouse.x, hot.mouse.y, pi, cicle, 0);
}

void open_item_description() {
	pushdescription push_info;
	auto push_last = last_item;
	last_item = (item*)hot.object;
	set_description("##%ItemName\n%ItemInformation");
	open_dialog(paint_item_description, true);
	last_item = push_last;
}

void open_game() {
	game_proc = 0;
	scene(paint_game);
}

void open_worldmap() {
	scene(paint_worldmap);
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

void initialize_ui() {
	set_cursor();
	ptips = paint_cursor;
	draw::syscursor(false);
}