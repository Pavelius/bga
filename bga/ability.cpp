#include "ability.h"
#include "bsdata.h"

static const char* format_percent = "%1i%%";
static const char* format_bonus = "%+1i";
static const char* format_d6 = "%+1id6";
static const char* format_weight = "%1i lb";

BSDATA(abilityi) = {
	{"Strenght", 1},
	{"Dexterity", 1},
	{"Constitution", 1},
	{"Intellegence", 1},
	{"Wisdow", 1},
	{"Charisma", 1},
	{"AC", 0},
	{"ArmorBonus", 0, format_bonus},
	{"DodgeBonus", 0, format_bonus},
	{"ArmorCheckPenalty", 0, format_bonus},
	{"SpellFailure", 0, format_percent},
	{"AttackMelee", 0, format_bonus},
	{"AttackRanged", 0, format_bonus},
	{"DamageMelee", 0},
	{"DamageRanged", 0},
	{"Fortitude", 0, format_bonus},
	{"Reflexes", 0, format_bonus},
	{"Will", 0, format_bonus},
	{"WeightAllowance", 0, format_weight},
	{"SneakAttack", 0, format_d6},
	{"ArmorProficiency", 0},
	{"SimpleWeaponMace", 0},
	{"SimpleWeaponMissile", 0},
	{"SimpleWeaponStaff", 0},
	{"SimpleWeaponSmallBlades", 0},
	{"MartialWeaponAxe", 0},
	{"MartialWeaponBow", 0},
	{"MartialWeaponFlail", 0},
	{"MartialWeaponGreatSword", 0},
	{"MartialWeaponHammer", 0},
	{"MartialWeaponLargeSword", 0},
	{"MartialWeaponPolearm", 0},
	{"HitPoints", 0},
	{"Stamina", 0},
};
assert_enum(abilityi, Stamina)

void statable::clear() {
	memset(this, 0, sizeof(*this));
}