#include "ability.h"
#include "bsdata.h"

BSDATA(skilli) = {
	{"Alchemy", Intelligence},
	{"AnimalEmpathy", Charisma},
	{"Bluff", Charisma},
	{"Concentration", Constitution},
	{"CraftWeapon", Strenght},
	{"Diplomacy", Charisma, {Bluff}},
	{"DisableDevice", Dexterity},
	{"Hide", Dexterity},
	{"Intimidate", Charisma, {Bluff}},
	{"KnowledgeArcana", Intelligence},
	{"KnowledgeHistory", Intelligence},
	{"KnowledgeReligion", Intelligence},
	{"MoveSilently", Dexterity},
	{"OpenLock", Dexterity},
	{"PickPocket", Dexterity, {Bluff}},
	{"Search", Intelligence},
	{"Spellcraft", Intelligence, {KnowledgeArcana}},
	{"Spot", Wisdow},
	{"UseMagicDevice", Intelligence},
	{"WildernessLore", Wisdow},
};
assert_enum(skilli, WildernessLore)

skilln skilli::getindex() const {
	return (skilln)(this - bsdata<skilli>::elements);
}