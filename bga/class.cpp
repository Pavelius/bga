#include "bsdata.h"
#include "class.h"

BSDATA(classi) = {
	{"Commoner", 0, 4, 2},
	{"Warrior", 0, 8, 2},
	{"Barbarian", 1, 10, 4},
	{"Bard", 1, 6, 2},
	{"Fighter", 1, 10, 2},
	{"Cleric", 1, 8, 2},
	{"Druid", 1, 8, 2},
	{"Paladin", 1, 10, 6},
	{"Ranger", 1, 8, 6},
	{"Rogue", 1, 6, 8},
	{"Sorcerer", 1, 4, 2},
	{"Wizard", 1, 4, 2},
};
assert_enum(classi, Wizard)

classn classi::getindex() const {
	return (classn)(this - bsdata<classi>::elements);
}

classn	classa::getmainclass() const {
	auto result = Commoner;
	auto value = 0;
	for(auto i = Commoner; i <= Wizard; i = (classn)(i + 1)) {
		if(classes[i] > value) {
			value = classes[i];
			result = i;
		}
	}
	return result;
}

int	classa::getlevel() const {
	auto result = 0;
	for(auto v : classes)
		result += v;
	return result;
}

unsigned classa::getnextlevel() const {
	auto new_level = getlevel() + 1;
	return new_level * (new_level - 1) * 500;
}

bool ischaracter(classn v) {
	switch(v) {
	case Cleric: case Fighter: case Ranger: case Rogue: case Sorcerer: case Wizard:
		return true;
	default:
		return false;
	}
}