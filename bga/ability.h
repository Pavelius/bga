#include "damage.h"
#include "feat.h"
#include "nameable.h"

#pragma once

enum ability_s : unsigned char {
	Strenght, Dexterity, Constitution, Intelligence, Wisdow, Charisma,
	AC, ArmorBonus, DodgeBonus, ArmorCheckPenalty, SpellFailure,
	Attack, AttackMelee, AttackRanged, Damage, DamageMelee, DamageRanged,
	Fortitude, Reflexes, Will,
	WeightAllowance,
	SneakAttack,
	ArmorProficiency,
	MartialWeaponAxe, MartialWeaponBow, MartialWeaponFlail, MartialWeaponGreatSword, MartialWeaponHammer,
	MartialWeaponLargeSword, MartialWeaponPolearm,
	HitPoints, Stamina,
};
struct abilityi : nameable {
	const char*	format;
};
struct statable {
	char		abilitites[Stamina + 1];
	damagea		resist;
	featf		feats;
	int			get(ability_s v) const { return abilitites[v]; }
	int			get(damage_s v) const { return resist[v]; }
	bool		is(feat_s v) const { return feats.is(v); }
	int			getbonus(ability_s v) const { return abilitites[v] / 2 - 5; }
};