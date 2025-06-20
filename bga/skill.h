#include "nameable.h"
#include "flagable.h"

#pragma once

enum abilityn : unsigned char;

enum skilln : unsigned char {
	Alchemy, AnimalEmpathy, Bluff, Concentration, CraftWeapon, Diplomacy,
	DisableDevice, Hide, Intimidate, KnowledgeArcana,
	MoveSilently, OpenLock, PickPocket, Search, SpellCraft, Spot,
	UseMagicDevice, WildernessLore,
};
typedef flagable<1, unsigned> skillf;
struct skilli : nameable {
	abilityn	ability;
	skilln		getindex() const;
};
struct skilla {
	char		skills[WildernessLore + 1];
};
