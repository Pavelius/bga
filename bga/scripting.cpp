#include "alignment.h"
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

template<> void ftscript<abilityi>(int value, int counter) {
	switch(modifier) {
	case Permanent: player->basic.abilities[value] += counter; break;
	default: player->abilities[value] += counter; break;
	}
}

template<> void ftscript<feati>(int value, int counter) {
	if(counter >= 0)
		player->feats.set(value);
}

template<> void ftscript<form>(int value, int counter) {
	auto& e = bsdata<form>::elements[value];
	execute(e.command, e.param1, e.param2, (void*)e.object);
}

template<> void ftscript<classi>(int value, int counter) {
	counter = get_bonus(counter);
	switch(modifier) {
	case Permanent:
		for(int i = 0; i < counter; i++)
			raise_class((classn)value);
		break;
	}
}

template<> void ftscript<itemi>(int value, int counter) {
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

template<> void ftscript<listi>(int value, int counter) {
	pushvalue push(modifier);
	script_run(bsdata<listi>::elements[value].elements);
}

template<> void ftscript<racei>(int value, int counter) {
	if(modifier==Permanent)
		player->race = (racen)value;
}

template<> void ftscript<alignmenti>(int value, int counter) {
	if(modifier == Permanent)
		player->alignment = (alignmentn)value;
}

template<> void ftscript<genderi>(int value, int counter) {
	if(modifier == Permanent)
		player->gender = (gendern)value;
}

template<> void ftscript<rfpma>(int value, int counter) {
	if(modifier == Permanent) {
		for(auto& e : player->resid) {
			if(e == 0xFFFF) {
				e = value;
				break;
			}
		}
	}
}

template<> void ftscript<npci>(int value, int counter) {
	auto pm = bsdata<npci>::elements + value;
	player->gender = pm->gender;
	player->race = pm->race;
	player->alignment = pm->alignment;
	memcpy(player->colors, pm->colors, sizeof(player->colors));
	create_abilities(true);
	pushvalue push(modifier, Permanent);
	script_run(bsdata<racei>::elements[player->race].elements);
	script_run(pm->elements);
	for(int i = 0; i < lenghtof(pm->resid); i++)
		player->resid[i] = getbsi(pm->resid[i]);
	player->feats.remove(DynamicAnimation);
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