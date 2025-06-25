#include "colorgrad.h"
#include "condition.h"
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

static bool if_spellcaster() {
	auto r = 0;
	for(auto& e : bsdata<classi>()) {
		if(!e.cast)
			continue;
		r += player->classes[e.getindex()];
	}
	return r > 0;
}

BSDATA(conditioni) = {
	{"IfSpellcaster", if_spellcaster},
};
BSDATAF(conditioni)
BSDATA(script) = {
	{"Damage", damage_change},
	{"Heal", heal},
};
BSDATAF(script)