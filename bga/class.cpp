#include "class.h"
#include "crt.h"

BSDATA(classi) = {
	{"Commoner"},
	{"Fighter"},
	{"Cleric"},
	{"Ranger"},
	{"Rogue"},
	{"Sorcerer"},
	{"Wizard"},
};
assert_enum(classi, Wizard)

class_s	classa::getmainclass() const {
	auto result = Commoner;
	auto value = 0;
	for(auto i = Commoner; i <= Wizard; i = (class_s)(i + 1)) {
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