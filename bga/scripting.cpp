#include "colorgrad.h"
#include "creature.h"
#include "draw.h"
#include "draw_command.h"
#include "draw_control.h"
#include "script.h"

using namespace draw;

static void test(int bonus) {
}

BSDATA(script) = {
	{"Test", test}
};
BSDATAF(script)

static void color_pick() {
	auto color_index = color_indecies[hot.param];
	if(color_index == -1)
		return;
	breakmodal(color_index);
}

static void default_color_pick() {
	breakmodal(default_color);
}

static void check_quick_weapon() {
	last_creature->weapon_index = (unsigned char)hot.param;
}

static void choose_creature_color() {
	auto push_default = default_color;
	auto index = hot.param;
	auto pi = bsdata<portraiti>::elements + last_creature->portrait;
	switch(index) {
	case 0: set_color("SkinNormal"); break;
	case 1: set_color("HairNormal"); break;
	default: set_color("HairNormal"); break;
	}
	default_color = pi->colors[index];
	last_creature->colors[index] = (unsigned char)form::choose("COLOR");
	default_color = push_default;
}

BSDATA(draw::command) = {
	{"Cancel", draw::buttoncancel, KeyEscape},
	{"CheckQuickWeapon", check_quick_weapon},
	{"ChooseCreatureColor", choose_creature_color},
	{"Close", draw::buttoncancel, KeyEscape},
	{"ColorPick", color_pick},
	{"DefaultColor", default_color_pick, KeyEscape},
	{"Done", draw::buttonok, KeyEnter},
};
BSDATAF(draw::command)