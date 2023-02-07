#include "nameable.h"

#pragma once

enum ability_s : unsigned char {
	Strenght, Dexterity, Constitution, Intelligence, Wisdow, Charisma,
	AC, Attack, AttackMelee, AttackRanged, Damage, DamageMelee, DamageRanged,
	HitPoints, Stamina,
};
struct abilityi : nameable {
};
struct statable {
	char		abilitites[Stamina + 1];
	int			get(ability_s v) const { return abilitites[v]; }
	int			getbonus(ability_s v) const { return 5 - abilitites[v] / 2; }
};