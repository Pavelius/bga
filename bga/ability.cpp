#include "ability.h"
#include "crt.h"

BSDATA(abilityi) = {
	{"Strenght"},
	{"Dexterity"},
	{"Constitution"},
	{"Intellegence"},
	{"Wisdow"},
	{"Charisma"},
	{"AC"},
	{"Attack"},
	{"AttackMelee"},
	{"AttackRanged"},
	{"Damage"},
	{"DamageMelee"},
	{"DamageRanged"},
	{"HitPoints"},
	{"Stamina"},
};
assert_enum(abilityi, Stamina)