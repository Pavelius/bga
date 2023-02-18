#include "ability.h"
#include "crt.h"

static const char* format_percent = "%1i%%";
static const char* format_bonus = "%+1i";
static const char* format_d6 = "%+1id6";

BSDATA(abilityi) = {
	{"Strenght"},
	{"Dexterity"},
	{"Constitution"},
	{"Intellegence"},
	{"Wisdow"},
	{"Charisma"},
	{"AC"},
	{"ArmorBonus", format_bonus},
	{"DodgeBonus", format_bonus},
	{"ArmorCheckPenalty", format_bonus},
	{"SpellFailure", format_percent},
	{"Attack", format_bonus},
	{"AttackMelee", format_bonus},
	{"AttackRanged", format_bonus},
	{"Damage"},
	{"DamageMelee"},
	{"DamageRanged"},
	{"SneakAttack", format_d6},
	{"ArmorProficiency"},
	{"MartialWeaponAxe"},
	{"MartialWeaponBow"},
	{"MartialWeaponFlail"},
	{"MartialWeaponGreatSword"},
	{"MartialWeaponHammer"},
	{"MartialWeaponLargeSword"},
	{"MartialWeaponPolearm"},
	{"HitPoints"},
	{"Stamina"},
};
assert_enum(abilityi, Stamina)