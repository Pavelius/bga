#pragma once

#include "nameable.h"

enum ability_s : unsigned char;
enum classn : unsigned char {
	Commoner,
	Fighter, Cleric, Rogue, Ranger, Sorcerer, Wizard,
	DomainClass = 64
};
struct classi : nameable {
	int			player;
	int			hit_points, skill_points;
	unsigned	skills;
	ability_s	cast;
	classn		getindex() const;
};
struct classa {
	char		classes[Wizard + 1];
	int			getlevel() const;
	classn		getmainclass() const;
	unsigned	getnextlevel() const;
};

bool ischaracter(classn v);

