#include "nameable.h"
#include "flagable.h"

#pragma once

enum abilityn : unsigned char;

enum skilln : unsigned char {
	Alchemy, AnimalEmpathy, Bluff, Concentration, CraftWeapon, Diplomacy,
	DisableDevice, Hide, Intimidate,
	KnowledgeArcana, KnowledgeHistory, KnowledgeReligion,
	MoveSilently, OpenLock, PickPocket, Search, Spellcraft, Spot,
	UseMagicDevice, WildernessLore,
};
typedef flag32 skillf;
struct skilli : nameable {
	abilityn	ability;
	skillf		synergy; // Get bonus +2 if skill have 5+ rang.
	skilln		getindex() const;
};
struct skilla {
	char		skills[WildernessLore + 1];
};
