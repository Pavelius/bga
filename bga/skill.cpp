#include "ability.h"
#include "bsdata.h"

BSDATA(skilli) = {
	{"Alchemy", Intelligence},
	{"AnimalEmpathy", Charisma},
	{"Bluff", Charisma},
	{"Concentration", Constitution},
	{"CraftWeapon", Strenght},
	{"Diplomacy", Charisma},
	{"DisableDevice", Dexterity},
	{"Hide", Dexterity},
	{"Intimidate", Charisma},
	{"KnowledgeArcana", Intelligence},
	{"MoveSilently", Dexterity},
	{"OpenLock", Dexterity},
	{"PickPocket", Dexterity},
	{"Search", Intelligence},
	{"Spellcraft", Intelligence},
	{"Spot", Wisdow},
	{"UseMagicDevice", Intelligence},
	{"WildernessLore", Wisdow},
};
assert_enum(skilli, WildernessLore)

skilln skilli::getindex() const {
	return (skilln)(this - bsdata<skilli>::elements);
}