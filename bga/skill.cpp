#include "ability.h"
#include "bsdata.h"

BSDATA(skilli) = {
	{"Alchemy", Intelligence},
	{"AnimalEmpathy", Charisma},
	{"Bluff", Charisma},
	{"Concentration", Constitution},
	{"Diplomacy", Charisma},
	{"DisableDevice", Dexterity},
	{"Hide", Dexterity},
	{"Intimidate", Charisma},
	{"Initiative", Dexterity},
	{"KnowledgeArcana", Intelligence},
	{"MoveSilently", Dexterity},
	{"OpenLock", Dexterity},
	{"PickPocket", Dexterity},
	{"Search", Intelligence},
	{"SpellCraft", Intelligence},
	{"UseMagicDevice", Intelligence},
	{"WildernessLore", Wisdow},
};
assert_enum(skilli, WildernessLore)