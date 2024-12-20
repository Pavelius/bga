#include "ability.h"
#include "bsdata.h"

static const char* format_percent = "%1i%%";
static const char* format_bonus = "%+1i";
static const char* format_d6 = "%+1id6";
static const char* format_weight = "%1i lb";

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
	{"AttackMelee", format_bonus},
	{"AttackRanged", format_bonus},
	{"DamageMelee"},
	{"DamageRanged"},
	{"Fortitude", format_bonus},
	{"Reflexes", format_bonus},
	{"Will", format_bonus},
	{"WeightAllowance", format_weight},
	{"SneakAttack", format_d6},
	{"ArmorProficiency"},
	{"SimpleWeaponMace"},
	{"SimpleWeaponMissile"},
	{"SimpleWeaponStaff"},
	{"SimpleWeaponSmallBlades"},
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

void statable::clear() {
	memset(this, 0, sizeof(*this));
}