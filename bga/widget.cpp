#include "bsreq.h"
#include "creature.h"
#include "colorgrad.h"
#include "crt.h"
#include "draw.h"
#include "draw_command.h"
#include "draw_gui.h"
#include "resinfo.h"
#include "script.h"
#include "widget.h"

using namespace draw;
using namespace res;

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
	image(gres(STON), 17, 0);
}

static void paint_empthy_offhand() {
	image(gres(STON), 13, 0);
}

static void quick_weapon_button() {
	gui.checked = (gui.value == last_creature->weapon_index);
	button_no_text();
}

static void quick_weapon_item() {
	auto pi = last_creature->wears + (QuickWeapon + gui.value * 2);
	image(gui.res, gui.value, 0);
	if(!(*pi)) {
		strokeout(paint_empthy_weapon, -2);
		return;
	}
}

static void quick_offhand_item() {
	auto pi = last_creature->wears + (QuickWeapon + gui.value * 2);
	image(gui.res, 8 + gui.value, 0);
	if(!(*pi)) {
		strokeout(paint_empthy_offhand, -2);
		return;
	}
}

static void portrait_large() {
	image(gres(PORTL), last_creature->portrait, 0);
}

static void portrait_small() {
	image(gres(PORTS), last_creature->portrait, 0);
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

BSDATA(widget) = {
	{"Background", background},
	{"Button", button},
	{"ButtonNT", button_no_text},
	{"ColorPicker", color_picker},
	{"CreatureAbility", creature_ability},
	{"CreatureColor", creature_color},
	{"Label", label},
	{"PortraitLarge", portrait_large},
	{"PortraitSmall", portrait_small},
	{"QuickWeaponButton", quick_weapon_button},
	{"QuickWeaponItem", quick_weapon_item},
	{"QuickOffhandItem", quick_offhand_item},
	{"Scroll", scroll},
};
BSDATAF(widget)