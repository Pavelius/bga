#include "nameable.h"
#include "flagable.h"

#pragma once

enum ability_s : unsigned char;

enum skilln : unsigned char {
	Alchemy, AnimalEmpathy, Bluff, Concentration, Diplomacy,
	DisableDevice, Hide, Intimidate, Initiative, KnowledgeArcana,
	MoveSilently, OpenLock, PickPocket, Search, SpellCraft,
	UseMagicDevice, WildernessLore,
};
typedef flagable<1, unsigned> skillf;
struct skilli : nameable {
	ability_s	ability;
};
struct skilla {
	char		skills[WildernessLore + 1];
};
