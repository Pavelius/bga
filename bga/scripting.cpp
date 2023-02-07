#include "colorgrad.h"
#include "creature.h"
#include "draw.h"
#include "draw_command.h"
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

BSDATA(draw::command) = {
	{"Cancel", draw::buttoncancel, KeyEscape},
	{"CheckQuickWeapon", check_quick_weapon},
	{"ColorPick", color_pick},
	{"DefaultColor", default_color_pick, KeyEscape},
	{"Done", draw::buttonok, KeyEnter},
};
BSDATAF(draw::command)