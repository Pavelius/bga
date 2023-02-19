#pragma once

enum class_s : unsigned char {
	Commoner,
	Fighter, Cleric, Rogue, Ranger, Sorcerer, Wizard,
};
struct classi {
	const char*		id;
	int				hit_points, skill_points;
	unsigned		skills;
};
struct classa {
	char			classes[Wizard + 1];
	int				getlevel() const;
	class_s			getmainclass() const;
	unsigned		getnextlevel() const;
};
bool				ischaracter(class_s v);

