#pragma once

#include "nameable.h"

enum abilityn : unsigned char;
enum classn : unsigned char {
	Commoner,
	Bard, Fighter, Cleric, Druid, Paladin, Ranger, Rogue, Sorcerer, Wizard,
};
struct classi : nameable {
	int			player;
	int			hit_points, skill_points;
	unsigned	skills;
	abilityn	cast;
	classn		getindex() const;
};
struct classa {
	char		classes[Wizard + 1];
	int			getlevel() const;
	classn		getmainclass() const;
	unsigned	getnextlevel() const;
};

bool ischaracter(classn v);

