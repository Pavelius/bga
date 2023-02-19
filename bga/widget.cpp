#include "area.h"
#include "bsreq.h"
#include "class.h"
#include "creature.h"
#include "colorgrad.h"
#include "crt.h"
#include "draw.h"
#include "draw_command.h"
#include "draw_control.h"
#include "draw_gui.h"
#include "draw_object.h"
#include "map.h"
#include "race.h"
#include "resinfo.h"
#include "script.h"
#include "scrolltext.h"
#include "widget.h"

using namespace draw;
using namespace res;

const char* getkg(int weight);
extern array console_data;

const int tile_size = 64;

static long current_tick;
static item drag_item;
static item *drag_item_source, *drag_item_dest;
static int current_info_tab;
static fnevent update_proc;
static bool need_update;
static char description_text[4096];
static scrolltext area_description;
stringbuilder description(description_text);

static void update_creature() {
	if(player)
		player->update();
}

static void update_item_description() {
	description.clear();
	last_item->getinfo(description);
}

static void update_creature_info() {
	if(!need_update)
		return;
	need_update = false;
	description.clear();
	switch(current_info_tab) {
	case 0: player->getinfo(description); break;
	case 2: player->getskillsinfo(description); break;
	default: break;
	}
}

static void invalidate_description() {
	need_update = true;
	area_description.invalidate();
}

static void form_opening() {
	invalidate_description();
}

static void set_value_and_update() {
	cbsetint();
	invalidate_description();
}

static void correct_camera() {
	if(camera.x + last_screen.width() > map::width * 16)
		camera.x = map::width * 16 - last_screen.width();
	if(camera.x < 0)
		camera.x = 0;
	if(camera.y + last_screen.width() > map::height * 12)
		camera.y = map::height * 16 - last_screen.height();
	if(camera.y < 0)
		camera.y = 0;
}

static void cursor_paint() {
	auto pi = gres(cursor.id);
	if(!pi)
		return;
	auto cicle = cursor.cicle;
	if(cursor.id == res::CURSORS) {
		auto pressed = hot.pressed;
		if(pressed)
			cicle += 1;
		image(hot.mouse.x, hot.mouse.y, pi, cicle, 0);
	} else {
		auto ti = pi->ganim(cursor.cicle, current_tick / 64);
		image(hot.mouse.x, hot.mouse.y, pi, ti, 0);
	}
}

static void paint_background() {
	current_tick = getcputime();
}

static void paint_logs(const char* format, int& origin, int& format_origin, int& maximum) {
	if(!format)
		return;
	rectpush push;
	if(format_origin == -1) {
		textfs(format);
		maximum = height;
		height = push.height;
		width = push.width;
		caret = push.caret;
		if(maximum > height)
			caret.y -= maximum - height;
	}
	textf(format, origin, format_origin);
}

void widget::initialize() {
	pbackground = paint_background;
	ptips = cursor_paint;
	form::opening = form_opening;
	cursor.set(res::CURSORS, 0);
	draw::syscursor(false);
}

void widget::open(const char* id) {
	auto p = bsdata<widget>::find(id);
	if(!p)
		return;
	scene(p->proc);
}

static res::token getanimation(race_s race, gender_s gender, class_s type, int ai, int& ws) {
	res::token icn;
	switch(race) {
	case Dwarf:
	case Gnome:
		icn = res::CDMB1;
		ws = 0;
		break;
	case Elf:
	case HalfElf:
		if(gender == Female)
			icn = res::CEFB1;
		else
			icn = res::CEMB1;
		ws = 2;
		break;
	case Halfling:
		if(gender == Female)
			icn = res::CIFB1;
		else
			icn = res::CIMB1;
		if(type == Wizard || type == Sorcerer)
			type = Rogue;
		if(ai > 1)
			ai = 1;
		ws = 0;
		break;
	default:
		if(gender == Female) {
			ws = 1;
			icn = res::CHFB1;
		} else {
			ws = 3;
			icn = res::CHMB1;
		}
		break;
	}
	if(type == Wizard || type == Sorcerer)
		icn = (res::token)(icn + (res::CDMW1 - res::CDMB1) + ai);
	else if(type == Cleric)
		icn = (res::token)(icn + ai);
	else if(type == Rogue && ai)
		icn = (res::token)(icn + (res::CDMT1 - res::CDMB1));
	else {
		if(ai == 3)
			icn = (res::token)(icn + 4);
		else
			icn = (res::token)(icn + ai);
	}
	return icn;
}

static int getarmorindex(const item& e) {
	auto v = e.geti().required;
	if(v.iskind<abilityi>() && v.value == ArmorProficiency)
		return v.counter;
	return 0;
}

static void painting_equipment(item equipment, int ws, int frame, unsigned flags, color* pallette) {
	if(!equipment)
		return;
	auto tb = equipment.geti().equiped;
	if(tb)
		image(gres(res::token(tb + ws)), frame, flags, pallette);
}

static void paperdoll(const coloration& colors, race_s race, gender_s gender, class_s type, int animation, int orientation, const item& armor, const item& weapon, const item& offhand, const item& helm) {
	sprite* source;
	unsigned flags;
	int ws;
	source = gres(getanimation(race, gender, type, getarmorindex(armor), ws));
	if(!source)
		return;
	const int directions = 9;
	int o = orientation;
	if(o >= directions) {
		flags = ImageMirrorH;
		o = (9 - 1) * 2 - o;
	} else
		flags = 0;
	color pallette[256]; colors.setpallette(pallette);
	auto frame = source->ganim(animation * directions + o, current_tick / 100);
	image(source, frame, flags, pallette);
	painting_equipment(weapon, ws, frame, flags, pallette);
	painting_equipment(helm, ws, frame, flags, pallette);
	painting_equipment(offhand, ws, frame, flags, pallette);
}

static void paperdoll() {
	static int orientation = 1;
	auto push_caret = caret;
	caret.x += width / 2;
	caret.y += height / 2 + 20;
	paperdoll(*player,
		player->race, player->gender, player->getmainclass(), 1, orientation,
		player->wears[Body], player->getweapon(), player->getoffhand(), player->wears[Head]);
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

static void background() {
	image(gui.res, gui.value, 0);
}

static const char* getname() {
	const char* pn = 0;
	if(gui.text)
		pn = gui.text;
	if(!pn && gui.id)
		pn = getnm(gui.id);
	if(!pn && gui.data)
		pn = gui.data.getname();
	if(!pn)
		pn = "";
	return pn;
}

static void interactive_execute() {
	if(gui.disabled)
		return;
	if(gui.data.iskind<command>()) {
		auto p = bsdata<command>::elements + gui.data.value;
		auto run = (hot.key == MouseLeft && !hot.pressed && gui.hilited);
		if(p->key && hot.key == p->key)
			run = true;
		if(run)
			execute(p->proc, gui.value);
	}
}

static void button(const sprite* p, unsigned short fiu, unsigned short fip) {
	rectpush push;
	auto& f = p->get(fiu);
	width = f.sx;
	height = f.sy;
	auto pressed = ishilite() && hot.pressed;
	if(gui.disabled)
		pressed = false;
	image(p, pressed ? fip : fiu, 0);
}

static void allow_disable_button() {
	if(gui.disabled)
		return;
	if(gui.data.iskind<command>()) {
		if(bsdata<command>::elements[gui.data.value].allow) {
			if(!bsdata<command>::elements[gui.data.value].allow())
				gui.disabled = true;
		}
	}
}

static void pressed_button() {
	button(gui.res, gui.frames[gui.disabled ? 3 : gui.checked ? 2 : 0], gui.frames[1]);
}

static void pressed_text() {
	auto push_caret = caret;
	auto push_fore = fore;
	if(gui.disabled)
		fore = fore.mix(colors::black);
	else if(hot.pressed && gui.hilited) {
		caret.x += 1;
		caret.y += 2;
	}
	texta(getname(), AlignCenterCenter);
	fore = push_fore;
	caret = push_caret;
}

static void button() {
	allow_disable_button();
	pressed_button();
	interactive_execute();
	pressed_text();
}

static void button_no_text() {
	allow_disable_button();
	pressed_button();
	interactive_execute();
}

static void textarea() {
	auto push_font = font;
	if(gui.res)
		font = gui.res;
	textf(getname());
	font = push_font;
}

static void label() {
	auto push_font = font;
	if(gui.res)
		font = gui.res;
	texta(getname(), AlignCenterCenter);
	font = push_font;
}

static void pressed_colorgrad(int index, int size) {
	auto push_caret = caret;
	if(hot.pressed && gui.hilited) {
		caret.x += 1 + size;
		caret.y += 1;
	}
	auto push_palt = palt; palt = pallette;
	set_color(pallette, 4, index);
	image(gres(res::COLGRAD), size, ImagePallette);
	palt = push_palt;
	caret = push_caret;
}

static void color_picker() {
	auto color_index = color_indecies[gui.value];
	pressed_button();
	if(color_index == -1)
		return;
	pressed_colorgrad(color_index, 0);
	interactive_execute();
}

static void creature_color() {
	auto color_index = player->colors[gui.value];
	pressed_button();
	if(color_index == -1)
		return;
	pressed_colorgrad(color_index, 1);
	interactive_execute();
}

static void scroll() {
	if(!gui.res)
		return;
	auto& f = gui.res->get(gui.frames[1]);
	auto w = f.sx;
	auto h = f.sy;
	auto sh = gui.res->get(gui.frames[4]).sy;
	auto push_caret = caret;
	button(gui.res, gui.frames[1], gui.frames[0]);
	caret.y = push_caret.y + height - h;
	button(gui.res, gui.frames[3], gui.frames[2]);
	caret.y = push_caret.y + h;
	//auto dy = height - h * 2 - sh;
	button(gui.res, gui.frames[4], gui.frames[5]);
	caret = push_caret;
}

static void paint_empthy_weapon() {
	if(gui.value == player->weapon_index)
		image(gres(ITEMS), 0, 0);
	else
		image(gres(STON), 17, 0);
}

static void paint_empthy_offhand() {
	image(gres(STON), 13, 0);
}

static void paint_empthy_gear() {
	image(gres(STON), gui.value, 0);
}

static void quick_weapon_button() {
	gui.checked = (gui.value == player->weapon_index);
	button_no_text();
}

static void paint_draggable() {
	if(drag_item_source) {
		if(hot.key == MouseRight
			|| hot.key == KeyEscape
			|| (hot.key == MouseLeft && hot.pressed))
			execute(buttoncancel);
	}
	drag_item_dest = 0;
	last_form->paint();
}

static creature* getowner(const item* p) {
	auto i = bsdata<creature>::source.indexof(p);
	return (i == -1) ? 0 : (creature*)bsdata<creature>::source.ptr(i);
}

static void begin_drag_item() {
	auto push_cursor = cursor;
	auto push_drag = drag_item_source;
	drag_item_source = (item*)hot.object;
	drag_item = *drag_item_source;
	drag_item_source->clear();
	update_creature();
	cursor.id = ITEMS;
	cursor.cicle = drag_item.geti().avatar * 2 + 1;
	scene(paint_draggable);
	if(!drag_item_dest)
		*drag_item_source = drag_item;
	else if(player->have(drag_item_dest)) {
		if(*drag_item_dest)
			player->additem(*drag_item_dest);
		*drag_item_dest = drag_item;
	} else {
		auto pn = getowner(drag_item_dest);
		if(pn)
			pn->additem(drag_item);
	}
	drag_item_source = push_drag;
	cursor = push_cursor;
	update_creature();
}

static void allow_drop_target(item* pi, wear_s slot) {
	if(drag_item_source) {
		if(gui.hilited) {
			drag_item_dest = pi;
			image(gui.res, 25, 0);
		}
	}
}

static void item_information() {
	auto push_last = last_item;
	last_item = (item*)hot.object;
	update_item_description();
	form::open("GIITMH08");
	last_item = push_last;
}

static void layer(color v) {
	auto push_alpha = alpha; alpha = 26;
	auto push_fore = fore; fore = v;
	rectf();
	alpha = push_alpha;
	fore = push_fore;
}

static void paint_minimap() {
	rectpush push;
	auto mm = map::getminimap();
	auto& sf = mm->get(0);
	caret.x += (width - sf.sx) / 2;
	caret.y += (height - sf.sy) / 2;
	width = sf.sx; height = sf.sy;
	image(mm, 0, 0);
	//rect rm;
	//rm.x1 = x2m(camera.x - camera_size.x / 2);
	//rm.y1 = y2m(camera.y - camera_size.y / 2);
	//rm.x2 = x2m(camera.x + camera_size.x / 2);
	//rm.y2 = y2m(camera.y + camera_size.y / 2);
	//rectb(rm, colors::white);
	for(auto p : party) {
		if(!p || !(*p))
			continue;
		//	auto position = p->getposition();
		//	auto x1 = x2m(position.x);
		//	auto y1 = y2m(position.y);
		//	circle(x1, y1, 2, colors::green);
	}
}

static void paint_tiles() {
	auto sp = map::getareasprite();
	if(!sp)
		return;
	int tx0 = camera.x / tile_size, ty0 = camera.y / tile_size;
	int dx = width / tile_size + 1, dy = height / tile_size + 1;
	int tx1 = tx0 + dx, ty1 = ty0 + dy;
	int ty = ty0;
	while(ty <= ty1) {
		int tx = tx0;
		while(tx <= tx1) {
			auto x = tx * tile_size - camera.x;
			auto y = ty * tile_size - camera.y;
			draw::image(x, y, sp, map::gettile(ty * 64 + tx), 0);
			tx++;
		}
		ty++;
	}
}

static void apply_shifer() {
	rect screen = {0, 0, getwidth(), getheight()};
	int index = -1;
	const int sz = 4;
	auto d = hot.mouse;
	if(d.x <= screen.x1)
		d.x = screen.x1;
	else if(d.x >= screen.x2 - 1)
		d.x = screen.x2 - 1;
	if(d.y <= screen.y1)
		d.y = screen.y1;
	else if(d.y >= screen.y2 - 1)
		d.y = screen.y2 - 1;
	if(d.x <= screen.x1 + sz)
		index = (d.y <= screen.y1 + sz) ? 3 : (d.y < screen.x2 - sz) ? 4 : 5;
	else if(d.x >= screen.x2 - sz)
		index = (d.y <= screen.y1 + sz) ? 7 : (d.y <= screen.y2 - sz) ? 0 : 1;
	else
		index = (d.y <= screen.x1 + sz) ? 2 : (d.y <= screen.y2 - sz) ? -1 : 6;
	if(index == -1) {
		if(!hot.mouse.in(last_screen))
			cursor.set(CURSORS, 0);
		else
			cursor.set(CURSORS, 4);
		return;
	}
	const int camera_step = 16;
	cursor.set(CURSARW, index);
	switch(index) {
	case 0: camera.x += camera_step; break;
	case 2: camera.y -= camera_step; break;
	case 4: camera.x -= camera_step; break;
	case 6: camera.y += camera_step; break;
	}
	correct_camera();
}

static void setup_visible_area() {
	last_screen.set(caret.x, caret.y, caret.x + width, caret.y + height);
	last_area = last_screen; last_area.move(camera.x, camera.y);
	last_area.offset(-128, -128);
}

static void area_map() {
	setup_visible_area();
	apply_shifer();
	paint_tiles();
}

static void paint_item(const item* pi) {
	if(!pi)
		return;
	auto push_caret = caret;
	setoffset(2, 2);
	if(!player->isusable(*pi))
		layer(colors::red);
	image(gres(ITEMS), pi->geti().avatar * 2, 0);
	caret = push_caret;
	if(!drag_item_source) {
		if(gui.hilited && hot.key == MouseRight && hot.pressed)
			execute(item_information, 0, 0, pi);
	}
}

static void paint_item_dragable(item* pi) {
	paint_item(pi);
	if(gui.hilited && !drag_item_source) {
		if(hot.key == MouseLeft && hot.pressed)
			execute(begin_drag_item, 0, 0, pi);
	}
}

static void backpack_button() {
	auto pi = player->wears + Backpack + gui.value;
	auto index = gui.value % 8;
	if(index >= 4)
		index += 4;
	image(gui.res, index, 0);
	allow_drop_target(pi, Backpack);
	if(*pi)
		paint_item_dragable(pi);
}

static void paint_drop_target(item* pi, wear_s slot) {
	if(drag_item_source && drag_item.canequip(slot)) {
		if(player->isusable(drag_item)) {
			if(gui.hilited) {
				image(gui.res, 25, 0);
				drag_item_dest = pi;
			} else
				image(gui.res, 16, 0);
		}
	}
}

static void gear_button() {
	auto type = (wear_s)(Head + gui.value);
	auto pi = player->wears + type;
	paint_drop_target(pi, type);
	if(*pi)
		paint_item_dragable(pi);
	else
		strokeout(paint_empthy_gear, -2);
}

static void paint_empthy_quiver() {
	image(gres(STON), 11, 0);
}

static void quiver_button() {
	auto pi = player->wears + Quiver + gui.value;
	image(gui.res, gui.value, 0);
	paint_drop_target(pi, Quiver);
	if(*pi)
		paint_item_dragable(pi);
	else
		strokeout(paint_empthy_quiver, -2);
}

static void quick_item_button() {
	auto pi = player->wears + QuickItem + gui.value;
	image(gui.res, gui.value, 0);
	paint_drop_target(pi, QuickItem);
	if(*pi)
		paint_item_dragable(pi);
}

static void quick_weapon_item() {
	auto pi = player->wears + (QuickWeapon + gui.value * 2);
	image(gui.res, gui.value, 0);
	paint_drop_target(pi, QuickWeapon);
	if(!(*pi))
		strokeout(paint_empthy_weapon, -2);
	else
		paint_item_dragable(pi);
	if(!drag_item_source) {
		if(gui.value == player->weapon_index)
			image(gres(STONSLOT), 34, 0);
	}
}

static void quick_offhand_item() {
	auto pi = player->wears + (QuickOffhand + gui.value * 2);
	image(gui.res, 8 + gui.value, 0);
	paint_drop_target(pi, QuickOffhand);
	if(!(*pi))
		strokeout(paint_empthy_offhand, -2);
	else
		paint_item_dragable(pi);
	if(!drag_item_source && pi && *pi && player->useoffhand()) {
		if(gui.value == player->weapon_index)
			image(gres(STONSLOT), 34, 0);
	}
}

static void portrait_large() {
	image(gres(PORTL), player->portrait, 0);
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
	rectpush push;
	if(selected_creatures.is(pc))
		hilight_protrait();
	setoffset(2, 2);
	image(gres(PORTS), pc->portrait, 0);
	if(drag_item_source && player != pc && draw::ishilite()) {
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
	rectpush push;
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

static void action_panel_na() {
	image(gres(GACTN), 1, 0);
	portrait_bar();
}

static void number() {
	auto push_font = font;
	if(gui.res)
		font = gui.res;
	char temp[64]; stringbuilder sb(temp);
	sb.add("%1i", gui.value);
	texta(temp, AlignCenterCenter);
	font = push_font;
}

static void creature_ability() {
	if(gui.data.iskind<abilityi>())
		gui.value = player->get((ability_s)gui.data.value);
	else if(equal(gui.id, "HPMax"))
		gui.value = player->hp_max;
	else if(equal(gui.id, "HP"))
		gui.value = player->hp;
	else if(equal(gui.id, "Coins"))
		gui.value = player->coins;
	number();
}

static void creature_ability_bonus() {
	if(gui.data.iskind<abilityi>())
		gui.value = player->getbonus((ability_s)gui.data.value);
	char temp[32]; stringbuilder sb(temp);
	auto push_fore = fore;
	if(gui.value > 0) {
		sb.add("%+1i", gui.value);
		fore = colors::green;
	} else if(gui.value < 0) {
		fore = colors::red;
		sb.add("%+1i", gui.value);
	} else
		sb.add("%+1i", gui.value);
	gui.text = temp;
	label();
	fore = push_fore;
}

static void creature_race() {
	gui.text = bsdata<racei>::elements[player->race].getname();
	label();
}

static void apply_weight_color() {
	switch(player->getencumbrance()) {
	case 3: case 2: fore = colors::red; break;
	case 1: fore = colors::yellow; break;
	default: break;
	}
}

static void creature_weight() {
	char temp[260]; stringbuilder sb(temp);
	sb.add(getkg(player->weight));
	sb.adds("%-1", getnm("From"));
	sb.adds(getkg(player->allowed_weight));
	gui.text = temp;
	apply_weight_color();
	label();
}

static void item_name() {
	gui.text = last_item->getname();
	label();
}

static void item_avatar() {
	auto i = last_item->geti().avatar * 2;
	image(caret.x + width / 2, caret.y + height / 2, gres(ITEMS), i + 1, 0);
}

static void item_action_button() {
}

static void paint_form() {
	bsdata<form>::elements[gui.data.value].paint();
}

static void hot_key() {
	auto ps = (command*)gui.data;
	if(!ps || !ps->key)
		return;
	if(hot.key == ps->key)
		execute(ps->proc, gui.value, 0, 0);
}

static void button_check(int& value) {
	gui.checked = (value == gui.value);
	pressed_button();
	auto run = (hot.key == MouseLeft && !hot.pressed && gui.hilited);
	if(hot.key == ('1' + gui.value))
		run = true;
	if(run)
		execute(set_value_and_update, gui.value, 0, &value);
}

static void button_info_tab() {
	button_check(current_info_tab);
}

static void text_description() {
	auto push_font = font;
	if(gui.res)
		font = gui.res;
	area_description.paint(description_text);
	if(gui.hilited)
		area_description.input();
	font = push_font;
}

void util_items_list();

BSDATA(widget) = {
	{"ActionPanelNA", action_panel_na},
	{"AreaMinimap", paint_minimap},
	{"AreaMap", area_map},
	{"Background", background},
	{"BackpackButton", backpack_button},
	{"Button", button},
	{"ButtonInfoTab", button_info_tab},
	{"ButtonNT", button_no_text},
	{"ColorPicker", color_picker},
	{"CreatureAbility", creature_ability},
	{"CreatureAbilityBonus", creature_ability_bonus},
	{"CreatureColor", creature_color},
	{"CreatureRace", creature_race},
	{"CreatureWeight", creature_weight},
	{"TextDescription", text_description},
	{"Form", paint_form},
	{"ItemActionButton", item_action_button},
	{"ItemAvatar", item_avatar},
	{"ItemName", item_name},
	{"GearButton", gear_button},
	{"HotKey", hot_key},
	{"Label", label},
	{"Paperdoll", paperdoll},
	{"PortraitLarge", portrait_large},
	{"QuickItemButton", quick_item_button},
	{"QuickWeaponButton", quick_weapon_button},
	{"QuickWeaponItem", quick_weapon_item},
	{"QuickOffhandItem", quick_offhand_item},
	{"QuiverButton", quiver_button},
	{"Scroll", scroll},
	{"UpdateCreatureInfo", update_creature_info},
#ifdef _DEBUG
	{"ItemList", util_items_list},
#endif // _DEBUG
};
BSDATAF(widget)