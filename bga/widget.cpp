#include "bsreq.h"
#include "creature.h"
#include "colorgrad.h"
#include "crt.h"
#include "draw.h"
#include "draw_command.h"
#include "draw_control.h"
#include "draw_gui.h"
#include "resinfo.h"
#include "script.h"
#include "widget.h"

using namespace draw;
using namespace res;

static item drag_item;
static item *drag_item_source, *drag_item_dest;
static char description_text[4096];

static void update_creature() {
	if(last_creature)
		last_creature->update();
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

void widget::initialize() {
	pfinish = cursor_paint;
	cursor.set(res::CURSORS, 0);
	draw::syscursor(false);
}

void widget::open(const char* id) {
	auto p = bsdata<widget>::find(id);
	if(!p)
		return;
	scene(p->proc);
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
	image(p, pressed ? fip : fiu, 0);
}

static void pressed_button() {
	button(gui.res, gui.frames[gui.checked ? 2 : 0], gui.frames[1]);
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
	set_color(4, index);
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
	auto color_index = last_creature->colors[gui.value];
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
	if(gui.value == last_creature->weapon_index)
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
	gui.checked = (gui.value == last_creature->weapon_index);
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
	else {
		if(*drag_item_dest)
			last_creature->additem(*drag_item_dest);
		*drag_item_dest = drag_item;
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
	form::open("GIITMH08");
	last_item = push_last;
}

static void paint_item(const item* pi) {
	if(!pi)
		return;
	auto push_caret = caret;
	setoffset(2, 2);
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
	auto pi = last_creature->wears + Backpack + gui.value;
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
		if(gui.hilited) {
			image(gui.res, 25, 0);
			drag_item_dest = pi;
		} else
			image(gui.res, 16, 0);
	}
}

static void gear_button() {
	auto type = (wear_s)(Head + gui.value);
	auto pi = last_creature->wears + type;
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
	auto pi = last_creature->wears + Quiver + gui.value;
	image(gui.res, gui.value, 0);
	paint_drop_target(pi, Quiver);
	if(*pi)
		paint_item_dragable(pi);
	else
		strokeout(paint_empthy_quiver, -2);
}

static void quick_item_button() {
	auto pi = last_creature->wears + QuickItem + gui.value;
	image(gui.res, gui.value, 0);
	paint_drop_target(pi, QuickItem);
	if(*pi)
		paint_item_dragable(pi);
}

static void quick_weapon_item() {
	auto pi = last_creature->wears + (QuickWeapon + gui.value * 2);
	image(gui.res, gui.value, 0);
	paint_drop_target(pi, QuickWeapon);
	if(!(*pi))
		strokeout(paint_empthy_weapon, -2);
	else
		paint_item_dragable(pi);
	if(!drag_item_source) {
		if(gui.value == last_creature->weapon_index)
			image(gres(STONSLOT), 34, 0);
	}
}

static void quick_offhand_item() {
	auto pi = last_creature->wears + (QuickOffhand + gui.value * 2);
	image(gui.res, 8 + gui.value, 0);
	paint_drop_target(pi, QuickOffhand);
	if(!(*pi))
		strokeout(paint_empthy_offhand, -2);
	else
		paint_item_dragable(pi);
}

static void portrait_large() {
	image(gres(PORTL), last_creature->portrait, 0);
}

static void hilight_protrait() {
	auto push_fore = fore;
	fore = colors::green;
	strokeout(rectb, -1);
	fore = push_fore;
}

static void portrait_small(creature* pc) {
	if(last_creature == pc)
		hilight_protrait();
	rectpush push;
	setoffset(2, 2);
	image(gres(PORTS), pc->portrait, 0);
}

static void choose_creature() {
	last_creature = (creature*)hot.object;
}

static void portrait_bar() {
	rectpush push;
	caret.x += 505; caret.y += 4;
	width = height = 46;
	for(auto i = 0; i < 6; i++) {
		portrait_small(party[i]);
		if(ishilite() && hot.key==MouseLeft && hot.pressed)
			execute(choose_creature, 0, 0, party[i]);
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
	if(equal(gui.id, "HPMax"))
		gui.value = last_creature->hp_max;
	else if(equal(gui.id, "HP"))
		gui.value = last_creature->hp;
	else if(equal(gui.id, "Coins"))
		gui.value = last_creature->coins;
	else {
		auto pn = bsdata<abilityi>::find(gui.id);
		if(!pn)
			return;
		auto i = pn - bsdata<abilityi>::elements;
		gui.value = last_creature->get((ability_s)i);
	}
	number();
}

static void item_name() {
	gui.text = last_item->getname();
	label();
}

static void item_avatar() {
	auto i = last_item->geti().avatar * 2;
	image(caret.x + width / 2, caret.y + height / 2, gres(ITEMS), i + 1, 0);
	//rectb();
}

static void item_description() {
	stringbuilder sb(description_text);
	last_item->getinfo(sb);
	gui.text = description_text;
	textarea();
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

void util_items_list();

BSDATA(widget) = {
	{"ActionPanelNA", action_panel_na},
	{"AreaMap", background},
	{"Background", background},
	{"BackpackButton", backpack_button},
	{"Button", button},
	{"ButtonNT", button_no_text},
	{"ColorPicker", color_picker},
	{"CreatureAbility", creature_ability},
	{"CreatureColor", creature_color},
	{"Form", paint_form},
	{"ItemActionButton", item_action_button},
	{"ItemAvatar", item_avatar},
	{"ItemName", item_name},
	{"ItemDescription", item_description},
	{"GearButton", gear_button},
	{"HotKey", hot_key},
	{"Label", label},
	{"PortraitLarge", portrait_large},
	{"QuickItemButton", quick_item_button},
	{"QuickWeaponButton", quick_weapon_button},
	{"QuickWeaponItem", quick_weapon_item},
	{"QuickOffhandItem", quick_offhand_item},
	{"QuiverButton", quiver_button},
	{"Scroll", scroll},
#ifdef _DEBUG
	{"ItemList", util_items_list},
#endif // _DEBUG
};
BSDATAF(widget)