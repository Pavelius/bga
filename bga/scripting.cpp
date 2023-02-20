#include "colorgrad.h"
#include "creature.h"
#include "draw.h"
#include "draw_command.h"
#include "draw_control.h"
#include "script.h"

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

BSDATA(script) = {
	{"Heal", heal},
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
	player->weapon_index = (unsigned char)hot.param;
}

static void choose_creature_color() {
	auto push_default = default_color;
	auto index = hot.param;
	auto pi = bsdata<portraiti>::elements + player->portrait;
	switch(index) {
	case 0: set_color("SkinNormal"); break;
	case 1: set_color("HairNormal"); break;
	default: set_color("HairNormal"); break;
	}
	default_color = pi->colors[index];
	player->colors[index] = (unsigned char)form::open("COLOR");
	default_color = push_default;
}

static void show_item_list() {
	widget::open("ItemList");
}

static void make_scrap() {
	heal(-1);
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

static void game_inventory() {
	open_form("GUIINV08");
}

static void game_journal() {
	open_form("GUIJRNL");
}

static void game_area_map() {
	open_form("GUIMAPAB");
}

static void game_options() {
	open_form("STONEOPT", true);
}

static void game_spells() {
	open_form("GUISPL");
}

static void game_charsheet() {
	open_form("GUIREC");
}

static void level_up() {
}

static bool allow_level_up() {
	return player->experience >= player->getnextlevel();
}

BSDATA(draw::command) = {
	{"Cancel", draw::buttoncancel, KeyEscape},
	{"CheckQuickWeapon", check_quick_weapon},
	{"ChooseCreatureColor", choose_creature_color},
	{"Close", draw::buttoncancel, KeyEscape},
	{"ColorPick", color_pick},
	{"DefaultColor", default_color_pick, KeyEscape},
	{"Done", draw::buttonok, KeyEnter},
	{"GameAreaMap", game_area_map, 'M'},
	{"GameCharsheet", game_charsheet, 'C'},
	{"GameInventory", game_inventory, 'I'},
	{"GameJournal", game_journal, 'J'},
	{"GameOptions", game_options, KeyEscape},
	{"GameSpells", game_spells, 'S'},
	{"LevelUp", level_up, 0, allow_level_up},
	{"MakeScrap", make_scrap, Ctrl + 'D'},
	{"ShowItemList", show_item_list, Ctrl + 'I'},
};
BSDATAF(draw::command)