#include "nameable.h"
#include "stringbuilder.h"

#pragma once

enum ability_s : unsigned char;

enum skill_s : unsigned char {
	Alchemy, AnimalEmpathy, Bluff, Concentration, Diplomacy,
	DisableDevice, Hide, Intimidate, Initiative, KnowledgeArcana,
	MoveSilently, OpenLock, PickPocket, Search, SpellCraft,
	UseMagicDevice, WildernessLore,
};
struct skilli : nameable {
	ability_s	ability;
	void		getinfo(stringbuilder& sb) const;
};
struct skilla {
	char		skills[WildernessLore + 1];
};
