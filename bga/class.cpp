#include "class.h"
#include "crt.h"

BSDATA(classi) = {
	{"Commoner", 4, 2},
	{"Fighter", 10, 2},
	{"Cleric", 8, 2},
	{"Ranger", 8, 6},
	{"Rogue", 6, 8},
	{"Sorcerer", 4, 2},
	{"Wizard", 4, 2},
};
assert_enum(classi, Wizard)

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