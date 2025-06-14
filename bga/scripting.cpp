#include "colorgrad.h"
#include "console.h"
#include "creature.h"
#include "draw.h"
#include "game.h"
#include "script.h"
#include "store.h"
#include "stringvar.h"
#include "timer.h"
#include "view.h"

using namespace draw;

extern stringbuilder sb;

void change_zoom_factor(int bonus);

static void heal(int bonus) {
	auto n = player->hp + bonus;
	if(n < 0)
		n = 0;
	else if(n > player->hp_max)
		n = player->hp_max;
	player->hp = n;
}

static void color_pick(int bonus) {
	auto color_index = color_indecies[hot.param];
	if(color_index == -1)
		return;
	breakmodal(color_index);
}

static void check_quick_weapon(int bonus) {
	player->weapon_index = (unsigned char)hot.param;
}

static void choose_creature_color(int bonus) {
	auto pi = bsdata<portraiti>::elements + player->portrait;
	switch(bonus) {
	case 0: set_color("SkinNormal"); break;
	case 1: set_color("HairNormal"); break;
	default: set_color("HairNormal"); break;
	}
	player->colors[bonus] = open_color_pick(player->colors[bonus], pi->colors[bonus]);
}

static void show_item_list(int bonus) {
}

static void open_store(int bonus) {
	last_store = bsdata<storei>::elements;
	// nextscene("GUISTBSB");
}

static void debug_test(int bonus) {
	print("Current timer %1i", current_game_tick);
	// open_store(bonus);
}

static void level_up(int bonus) {
}

static bool allow_level_up(int bonus) {
	return player->experience >= player->getnextlevel();
}

static void button_cancel(int bonus) {
	draw::breakmodal(bonus);
}

static void attack_change(int bonus) {
	fnscript<abilityi>(AttackMelee, bonus);
	fnscript<abilityi>(AttackRanged, bonus);
}

static void damage_change(int bonus) {
	fnscript<abilityi>(DamageMelee, bonus);
	fnscript<abilityi>(DamageRanged, bonus);
}

static bool allow_buy_item(int bonus) {
	if(!store_item)
		return false;
	return true;
}
static void buy_item_script(int bonus) {
}

static bool allow_sell_item(int bonus) {
	if(!party_item)
		return false;
	return true;
}
static void sell_item_script(int bonus) {
}

static bool allow_use_steal(int bonus) {
	if(!store_item)
		return false;
	return true;
}
static void use_steal(int bonus) {
}

BSDATA(script) = {
	{"Attack", attack_change},
	{"BuyItem", buy_item_script, allow_buy_item},
	{"Cancel", button_cancel},
	{"ChangeZoomFactor", change_zoom_factor},
	{"CheckQuickWeapon", check_quick_weapon},
	{"ChooseCreatureColor", choose_creature_color},
	{"Close", button_cancel},
	{"ColorPick", color_pick},
	{"Damage", damage_change},
	{"DebugTest", debug_test},
	{"Done", button_cancel},
	{"Heal", heal},
	{"LevelUp", level_up, allow_level_up},
	{"SellItem", sell_item_script, allow_sell_item},
	{"ShowItemList", show_item_list},
	{"UseSteal", use_steal, allow_use_steal},
};
BSDATAF(script)