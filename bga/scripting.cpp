#include "colorgrad.h"
#include "container.h"
#include "condition.h"
#include "console.h"
#include "creature.h"
#include "draw.h"
#include "form.h"
#include "game.h"
#include "itemground.h"
#include "list.h"
#include "npc.h"
#include "modifier.h"
#include "pushvalue.h"
#include "script.h"
#include "store.h"
#include "stringvar.h"
#include "timer.h"
#include "view.h"

using namespace draw;

static int get_bonus(int counter, int minimum = 1) {
	if(counter < minimum)
		counter = minimum;
	return counter;
}

template<> void fnscript<abilityi>(int value, int counter) {
	switch(modifier) {
	case Permanent: player->basic.abilities[value] += counter; break;
	default: player->abilities[value] += counter; break;
	}
}

template<> void fnscript<feati>(int value, int counter) {
	if(counter >= 0) {
		switch(modifier) {
		case Permanent: player->basic.feats.set(value); break;
		default: player->feats.set(value); break;
		}
	}
}

template<> void fnscript<form>(int value, int counter) {
	auto& e = bsdata<form>::elements[value];
	execute(e.command, e.param1, e.param2, (void*)e.object);
}

template<> void fnscript<itemi>(int value, int counter) {
	if(counter >= 0) {
		counter = get_bonus(counter);
		item it(value, counter);
		switch(modifier) {
		case InsideBackpack: player->additem(it); break;
		case InsideContainer: last_container->add(it); break;
		case InsideStore: last_store->add(it); break;
		default: player->equip(it); break;
		}
	}
}

template<> void fnscript<listi>(int value, int counter) {
	auto push_modifier = modifier;
	script_run(bsdata<listi>::elements[value].elements);
	modifier = push_modifier;
}

template<> void fnscript<npci>(int value, int counter) {
	auto pm = bsdata<npci>::elements + value;
	player->gender = pm->gender;
	player->race = pm->race;
	player->alignment = pm->alignment;
	player->npc = getbsi(pm);
	memcpy(player->colors, pm->colors, sizeof(player->colors));
}

static void damage_change(int bonus) {
	fnscript<abilityi>(DamageMelee, bonus);
	fnscript<abilityi>(DamageRanged, bonus);
}

static void heal(int bonus) {
	auto n = player->hp + bonus;
	if(n < 0)
		n = 0;
	else if(n > player->hp_max)
		n = player->hp_max;
	player->hp = n;
}

static void party_member(int bonus) {
	if(bonus >= lenghtof(party))
		bonus = lenghtof(party) - 1;
	player = party[bonus];
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

static void identify_item(int bonus) {
	last_item->identify(bonus);
}

BSDATA(conditioni) = {
	{"IfSpellcaster", if_spellcaster},
};
BSDATAF(conditioni)
BSDATA(script) = {
	{"Damage", damage_change},
	{"IdentifyItem", identify_item},
	{"Heal", heal},
	{"PartyMember", party_member},
};
BSDATAF(script)