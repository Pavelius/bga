#include "collection.h"
#include "class.h"
#include "crt.h"
#include "portrait.h"
#include "race.h"

BSDATA(portraiti) = {
	{"NoPortrait"},
	{"DFF", {12, 2, 41, 65}, Female, Dwarf, Fighter},
	{"DMC", {84, 2, 39, 27}, Male, Dwarf, Cleric},
	{"DMF", {84, 3, 38, 27}, Male, Dwarf, Fighter},
	{"DMT", {84, 1, 41, 49}, Male, Dwarf, Rogue},
	{"EFC", {13, 2, 47, 60}, Female, Elf, Cleric},
	{"EFW", {10, 2, 54, 66}, Female, Elf, Wizard},
	{"EMF", {84, 2, 41, 26}, Male, Elf, Fighter},
	{"EMF2", {12, 0, 39, 41}, Male, Elf, Fighter},
	{"EMT", {12, 2, 66, 37}, Male, Elf, Rogue},
	{"EMW", {13, 6, 51, 49}, Male, Elf, Wizard},
	{"GFW", {85, 0, 40, 58}, Female, Gnome, Wizard},
	{"GMT", {12, 0, 66, 14}, Male, Gnome, Rogue},
	{"GMW", {84, 2, 41, 46}, Male, Gnome, Wizard},
	{"HFF", {84, 2, 100, 98}, Female, Human, Fighter},
	{"HFT", {84, 0, 39, 66}, Female, Human},
	{"HFW", {12, 0, 49, 66}, Female, Human},
	{"HMB", {87, 0, 41, 49}, Male, Human, Fighter},
	{"HMC", {12, 0, 49, 53}, Male, Human},
	{"HMF2", {87, 0, 66, 98}, Male, Human, Fighter},
	{"HMF", {84, 0, 66, 27}, Male, Human, Fighter},
	{"HMW", {12, 5, 58, 48}, Male, Human},
	{"HaFF", {84, 2, 39, 49}, Female, Halfling, Fighter},
	{"HaFT", {12, 0, 39, 14}, Female, Halfling},
	{"HaMF", {84, 2, 39, 24}, Male, Halfling, Fighter},
	{"HaMT", {84, 2, 41, 56}, Male, Halfling},
	{"HeFB", {12, 2, 39, 56}, Female, HalfElf, Fighter},
	{"HeFC", {12, 3, 39, 58}, Female, HalfElf},
	{"HeFF", {84, 2, 41, 66}, Female, HalfElf, Fighter},
	{"HeFW", {84, 0, 41, 40}, Female, HalfElf},
	{"HeMF", {84, 1, 41, 98}, Male, HalfElf, Fighter},
	{"HeMT", {85, 2, 41, 54}, Male, HalfElf},
	{"HeMW", {12, 0, 49, 41}, Male, HalfElf},
	{"HFW2", {13, 0, 66, 66}, Female, Human},
	{"HMF3", {8, 0, 66, 103}, Male, Human, Fighter},
	{"EFW2", {83, 0, 60, 60}, Female, Elf},
	{"HFW3", {8, 0, 66, 60}, Female, Human},
	{"EFF", {12, 0, 66, 25}, Female, Elf, Fighter},
	{"HMF4", {107, 0, 66, 102}, Male, Human, Fighter},
	{"EMC", {83, 110, 49, 60}, Male, Elf},
	{"HMW2", {12, 0, 22, 41}, Male, Human},
	{"HMW3", {84, 0, 46, 46}, Male, Human},
	{"HMC2", {87, 0, 63, 60}, Male, Human},
	{"HMC3", {12, 79, 94, 91}, Male, Human},
	{"HMF5", {12, 3, 41, 66}, Male, Human, Fighter},
	{"HMB2", {84, 2, 39, 47}, Male, Human, Fighter},
	{"DMF2", {12, 3, 66, 58}, Male, Dwarf, Fighter},
	{"HFT2", {12, 2, 39, 63}, Female, Human},
	{"HFD", {12, 2, 41, 49}, Female, Human},
	{"HFF2", {12, 2, 41, 37}, Female, Human, Fighter},
	{"HMC4", {12, 1, 39, 56}, Male, Human},
	{"HMF6", {84, 0, 66, 47}, Male, Human, Fighter},
	{"HMF7", {8, 0, 41, 46}, Male, Human, Fighter},
	{"HMW4", {13, 0, 107, 66}, Male, Human},
	{"2FBAR1", {84, 3, 41, 40}, Female, Human, Fighter},
	{"2FDEF1", {0, 79, 66, 41}, Female, Elf, Fighter},
	{"2FELF1", {85, 0, 39, 56}, Female, Elf, Fighter},
	{"2FELF2", {13, 0, 66, 63}, Female, Elf, Fighter},
	{"2FELF3", {8, 4, 51, 53}, Female, Elf, Fighter},
	{"2FGNM1", {107, 79, 50, 51}, Female, Gnome, Fighter},
	{"2FHUM1", {13, 0, 49, 66}, Female, Human, Fighter},
	{"2FHUM2", {85, 0, 41, 40}, Female, Human, Fighter},
	{"2FHUM3", {84, 2, 46, 45}, Female, Human, Fighter},
	{"2FORC1", {85, 4, 39, 38}, Female, HalfOrc, Fighter},
	{"2FTIF1", {14, 0, 66, 41}, Female, Human, Fighter},
	{"2MAAS1", {12, 3, 50, 39}, Male, Human, Cleric},
	{"2MDEF1", {0, 79, 66, 54}, Male, Elf, Fighter},
	{"2MDEF2", {0, 79, 66, 56}, Male, Elf, Fighter},
	{"2MDWF1", {85, 0, 41, 46}, Male, Dwarf, Fighter},
	{"2MELF1", {13, 6, 62, 66}, Male, Elf, Fighter},
	{"2MGNM1", {107, 79, 37, 39}, Male, Gnome, Fighter},
	{"2MHUM1", {10, 0, 39, 63}, Male, Human, Fighter},
	{"2MHUM2", {10, 6, 49, 66}, Male, Human, Fighter},
	{"2MHUM3", {85, 0, 66, 49}, Male, Human, Fighter},
	{"2MHUM4", {85, 0, 41, 66}, Male, Human, Fighter},
	{"2MHUM5", {10, 2, 65, 39}, Male, Human, Fighter},
	{"2MORC1", {112, 0, 66, 41}, Male, HalfOrc, Fighter},
	{"2MPAL1", {84, 2, 41, 66}, Male, Human, Fighter},
	{"2FHUM4", {85, 0, 50, 39}, Female, Human, Fighter},
	{"2MBAR1", {85, 0, 41, 39}, Male, Human, Fighter},
	{"2MDWF2", {85, 2, 39, 56}, Male, Dwarf, Fighter},
	{"2MHAF1", {85, 2, 66, 38}, Male, Halfling, Fighter},
	{"2MHUM6", {85, 2, 41, 39}, Male, Human, Fighter},
	{"2MHUM7", {8, 0, 39, 66}, Male, Human, Fighter},
};
BSDATAF(portraiti)

static bool filter_gender(const void* object, int param) {
	return ((portraiti*)object)->gender == (gender_s)param;
}

short unsigned random_portrait(gender_s gender, portraiti** exclude) {
	collection<portraiti> source;
	source.select();
	source.match(filter_gender, gender, true);
	if(exclude) {
		for(auto p = exclude; *p; p++)
			source.remove(*p);
	}
	if(!source)
		return 0;
	return source.random() - bsdata<portraiti>::elements;
}