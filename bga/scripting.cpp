#include "colorgrad.h"
#include "console.h"
#include "creature.h"
#include "draw.h"
#include "draw_control.h"
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
	player->colors[bonus] = (unsigned char)form::open("COLOR");
	default_color = push_default;
}

static void show_item_list(int bonus) {
	widget::open("ItemList");
}

static void debug_test(int bonus) {
	logm("Current timer %1i", current_game_tick);
}

static void open_form(const char* id, bool open_only_in_game = false) {
	if(open_only_in_game) {
		if(last_form == bsdata<form>::find("GGAME"))
			form::nextscene(id);
		else
			form::nextscene("GGAME");
	} else {
		if(last_form == bsdata<form>::find(id))
			id = "GGAME";
		form::nextscene(id);
	}
}

static void game_inventory(int bonus) {
	open_form("GUIINV08");
}

static void game_journal(int bonus) {
	open_form("GUIJRNL");
}

static void game_area_map(int bonus) {
	open_form("GUIMAPAB");
}

static void game_options(int bonus) {
	open_form("STONEOPT", true);
}

static void game_spells(int bonus) {
	open_form("GUISPL");
}

static void game_charsheet(int bonus) {
	open_form("GUIREC");
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
	{"GameAreaMap", game_area_map},
	{"GameCharsheet", game_charsheet},
	{"GameInventory", game_inventory},
	{"GameJournal", game_journal},
	{"GameOptions", game_options},
	{"GameSpells", game_spells},
	{"Heal", heal},
	{"LevelUp", level_up, allow_level_up},
	{"ShowItemList", show_item_list},
	{"SelectAll", select_all},
};
BSDATAF(script)