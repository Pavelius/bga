#include "damage.h"
#include "feat.h"
#include "skill.h"

#pragma once

enum abilityn : unsigned char {
	Strenght, Dexterity, Constitution, Intelligence, Wisdow, Charisma,
	AC, ArmorBonus, DodgeBonus, ArmorCheckPenalty, SpellFailure,
	Attack, AttackMelee, AttackRanged, DamageMelee, DamageRanged,
	Fortitude, Reflexes, Will,
	SaveVsCharm, SaveVsIllusion, SaveVsFear, SaveVsPoison, SaveVsSpells,
	WeightAllowance,
	SneakAttack,
	SkillPoints, HitPoints, Stamina,
};
struct abilityi : nameable {
	int			player;
	const char*	format;
};
struct statable : skilla {
	char		abilities[Stamina + 1];
	damagea		resist;
	featf		feats;
	void		clear();
	int			get(abilityn v) const { return abilities[v]; }
	int			get(damagen v) const { return resist[v]; }
	int			get(skilln v) const { return skills[v]; }
	bool		is(featn v) const { return feats.is(v); }
	int			getbonus(abilityn v) const { return abilities[v] / 2 - 5; }
};