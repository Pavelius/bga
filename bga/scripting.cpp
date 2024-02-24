#include "colorgrad.h"
#include "console.h"
#include "creature.h"
#include "draw.h"
#include "draw_form.h"
#include "script.h"
#include "timer.h"

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
	form::open("GUIWBTP", false);
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

static void attack_change(int bonus) {
	fnscript<abilityi>(AttackMelee, bonus);
	fnscript<abilityi>(AttackRanged, bonus);
}

static void damage_change(int bonus) {
	fnscript<abilityi>(DamageMelee, bonus);
	fnscript<abilityi>(DamageRanged, bonus);
}

BSDATA(script) = {
	{"Attack", attack_change},
	{"Cancel", button_cancel},
	{"ChangeZoomFactor", change_zoom_factor},
	{"CheckQuickWeapon", check_quick_weapon},
	{"ChooseCreatureColor", choose_creature_color},
	{"Close", button_cancel},
	{"ColorPick", color_pick},
	{"Damage", damage_change},
	{"DebugTest", debug_test},
	{"DefaultColor", default_color_pick},
	{"Done", button_cancel},
	{"Heal", heal},
	{"LevelUp", level_up, allow_level_up},
	{"ShowItemList", show_item_list},
	{"SelectAll", select_all},
};
BSDATAF(script)