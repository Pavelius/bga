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
	if(player->hp > 0)
		player->hp--;
}

static void open_form(const char* id) {
	form::nextscene(id);
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
	open_form("STONEOPT");
}

static void game_spells() {
	open_form("GUISPL");
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
	{"GameInventory", game_inventory, 'I'},
	{"GameJournal", game_journal, 'J'},
	{"GameOptions", game_options, KeyEscape},
	{"GameSpells", game_spells, 'S'},
	{"MakeScrap", make_scrap, Ctrl + 'D'},
	{"ShowItemList", show_item_list, Ctrl + 'I'},
};
BSDATAF(draw::command)