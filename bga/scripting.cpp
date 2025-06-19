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

static void heal(int bonus) {
	auto n = player->hp + bonus;
	if(n < 0)
		n = 0;
	else if(n > player->hp_max)
		n = player->hp_max;
	player->hp = n;
}

static void debug_test(int bonus) {
	print("Current timer %1i", current_game_tick);
	// open_store(bonus);
}

static void attack_change(int bonus) {
	fnscript<abilityi>(AttackMelee, bonus);
	fnscript<abilityi>(AttackRanged, bonus);
}

static void damage_change(int bonus) {
	fnscript<abilityi>(DamageMelee, bonus);
	fnscript<abilityi>(DamageRanged, bonus);
}

static void proficient_swords_and_bow(int bonus) {
	if(!player->abilities[MartialWeaponBow])
		player->abilities[MartialWeaponBow]++;
	if(!player->abilities[MartialWeaponLargeSword])
		player->abilities[MartialWeaponLargeSword]++;
}

BSDATA(script) = {
	{"Attack", attack_change},
	{"Damage", damage_change},
	{"DebugTest", debug_test},
	{"Heal", heal},
	{"ProficientSwordsAndBows", proficient_swords_and_bow},
};
BSDATAF(script)