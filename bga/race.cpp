#include "bsdata.h"
#include "race.h"

BSDATA(racei) = {
	{"Human"},
	{"Dwarf"},
	{"Elf"},
	{"Gnome"},
	{"HalfElf"},
	{"Halfling"},	
	{"HalfOrc"},
	{"Goblin"},
};
assert_enum(racei, Goblin)