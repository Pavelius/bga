#pragma once

#include "nameable.h"

enum abilityn : unsigned char;
enum alignmentn : unsigned char;
enum skilln : unsigned char;
enum classn : unsigned char {
	Commoner, Warrior,
	Barbarian, Bard, Fighter, Cleric, Druid, Paladin, Ranger, Rogue, Sorcerer, Wizard,
};
struct classi : nameable {
	int			player;
	int			hit_points, skill_points, attack;
	unsigned	skills, alignment, saves;
	abilityn	cast;
	classn		getindex() const;
	bool		is(alignmentn v) const { return (alignment & (1 << v)) != 0; }
	bool		is(skilln v) const { return (skills & (1 << v)) != 0; }
	bool		is(abilityn v) const { return (saves & (1 << v)) != 0; }
};
struct classa {
	char		classes[Wizard + 1];
	bool		isalignment(alignmentn v) const { return true; }
	int			getlevel() const;
	classn		getmainclass() const;
	unsigned	getnextlevel() const;
	int			getmaxskill() const { return getlevel() + 3; }
};

bool ischaracter(classn v);

