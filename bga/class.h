#pragma once

enum classn : unsigned char {
	Commoner,
	Fighter, Cleric, Rogue, Ranger, Sorcerer, Wizard,
};
struct classi {
	const char*	id;
	int			hit_points, skill_points;
	unsigned	skills;
};
struct classa {
	char		classes[Wizard + 1];
	int			getlevel() const;
	classn		getmainclass() const;
	unsigned	getnextlevel() const;
};
bool			ischaracter(classn v);

