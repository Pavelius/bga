#include "colorgrad.h"
#include "console.h"
#include "creature.h"
#include "draw.h"
#include "game.h"
#include "modifier.h"
#include "script.h"
#include "store.h"
#include "stringvar.h"
#include "timer.h"
#include "view.h"

using namespace draw;

template<> void ftscript<abilityi>(int value, int counter) {
	switch(modifier) {
	case Permanent: player->basic.abilities[value] += counter; break;
	default: player->abilities[value] += counter; break;
	}
}

template<> void ftscript<feati>(int value, int counter) {
	if(counter >= 0) {
		switch(modifier) {
		case Permanent: player->basic.feats.set(value); break;
		default: player->feats.set(value); break;
		}
	}
}

static void attack_change(int bonus) {
	ftscript<abilityi>(AttackMelee, bonus);
	ftscript<abilityi>(AttackRanged, bonus);
}

static void damage_change(int bonus) {
	ftscript<abilityi>(DamageMelee, bonus);
	ftscript<abilityi>(DamageRanged, bonus);
}

static void heal(int bonus) {
	auto n = player->hp + bonus;
	if(n < 0)
		n = 0;
	else if(n > player->hp_max)
		n = player->hp_max;
	player->hp = n;
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
	{"Heal", heal},
	{"ProficientSwordsAndBows", proficient_swords_and_bow},
};
BSDATAF(script)