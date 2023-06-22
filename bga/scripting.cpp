#include "colorgrad.h"
#include "console.h"
#include "creature.h"
#include "draw.h"
#include "draw_form.h"
#include "script.h"
#include "timer.h"

using namespace draw;

extern stringbuilder sb;

static void heal(int bonus) {
	auto n = player->hp + bonus;
	if(n < 0)
		n = 0;
	else if(n > player->hp_max)
		n = player->hp_max;
	player->hp = n;
}

static void select_all(int bonus) {
	selected_creatures.clear();
	for(auto p : party)
		selected_creatures.add(p);
}

static void color_pick(int bonus) {
	auto color_index = color_indecies[hot.param];
	if(color_index == -1)
		return;
	breakmodal(color_index);
}

static void default_color_pick(int bonus) {
	breakmodal(default_color);
}

static void check_quick_weapon(int bonus) {
	player->weapon_index = (unsigned char)hot.param;
}

static void choose_creature_color(int bonus) {
	auto push_default = default_color;
	auto pi = bsdata<portraiti>::elements + player->portrait;
	switch(bonus) {
	case 0: set_color("SkinNormal"); break;
	case 1: set_color("HairNormal"); break;
	default: set_color("HairNormal"); break;
	}
	default_color = pi->colors[bonus];
	player->colors[bonus] = (unsigned char)form::open("COLOR", true);
	default_color = push_default;
}

static void show_item_list(int bonus) {
}

static void debug_test(int bonus) {
	logm("Current timer %1i", current_game_tick);
}

static void level_up(int bonus) {
}

static bool allow_level_up(int bonus) {
	return player->experience >= player->getnextlevel();
}

static void button_cancel(int bonus) {
	draw::breakmodal(bonus);
}

BSDATA(script) = {
	{"Cancel", button_cancel},
	{"CheckQuickWeapon", check_quick_weapon},
	{"ChooseCreatureColor", choose_creature_color},
	{"Close", button_cancel},
	{"ColorPick", color_pick},
	{"DebugTest", debug_test},
	{"DefaultColor", default_color_pick},
	{"Done", button_cancel},
	{"Heal", heal},
	{"LevelUp", level_up, allow_level_up},
	{"ShowItemList", show_item_list},
	{"SelectAll", select_all},
};
BSDATAF(script)