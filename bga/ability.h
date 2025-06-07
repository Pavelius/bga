#include "damage.h"
#include "feat.h"
#include "skill.h"

#pragma once

enum abilityn : unsigned char {
	Strenght, Dexterity, Constitution, Intelligence, Wisdow, Charisma,
	AC, ArmorBonus, DodgeBonus, ArmorCheckPenalty, SpellFailure,
	AttackMelee, AttackRanged, DamageMelee, DamageRanged,
	Fortitude, Reflexes, Will,
	WeightAllowance,
	SneakAttack,
	ArmorProficiency,
	SimpleWeaponMace, SimpleWeaponMissile, SimpleWeaponStaff, SimpleWeaponSmallBlades,
	MartialWeaponAxe, MartialWeaponBow, MartialWeaponFlail, MartialWeaponGreatSword, MartialWeaponHammer,
	MartialWeaponLargeSword, MartialWeaponPolearm,
	HitPoints, Stamina,
};
struct abilityi : nameable {
	int			player;
	const char*	format;
};
struct statable : skilla {
	char		abilitites[Stamina + 1];
	damagea		resist;
	featf		feats;
	void		clear();
	int			get(abilityn v) const { return abilitites[v]; }
	int			get(damagen v) const { return resist[v]; }
	int			get(skilln v) const { return skills[v]; }
	bool		is(feat_s v) const { return feats.is(v); }
	int			getbonus(abilityn v) const { return abilitites[v] / 2 - 5; }
};