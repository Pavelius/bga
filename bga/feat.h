#include "flagable.h"
#include "nameable.h"
#include "variant.h"

#pragma once

enum featgf : unsigned char {
	GeneralFeat, FighterFeat, UpgradeFeat
};
enum featn : unsigned char {
	Enemy,
	SimpleWeaponMace, SimpleWeaponMissile, SimpleWeaponStaff, SimpleWeaponDagger,
	MartialWeaponAxe, MartialWeaponBow, MartialWeaponFlail, MartialWeaponGreatSword, MartialWeaponHammer,
	MartialWeaponSword, MartialWeaponPolearm,
	ArmorProficiencyLight, ArmorProficiencyMedium, ArmorProficiencyHeavy, ShieldProficiency,
	Alertness, Ambidexterity,
	BlindFight,
	Cleave, CombatCasting, CombatExpertise, CombatReflexes,
	DeadlyPercision, DeflectArrows, Dodge, Endurance,
	FarShoot,
	GreateFortitude,
	ImprovedCritical, ImprovedDisarm, ImprovedFeint, ImprovedInitiative, ImprovedOverrun, ImprovedSunder, ImprovedTrip, ImprovedUnarmedStrike,
	IronWill,
	LuckOfHeroes, Leadership, LightingReflexes,
	Mobility, Multiattack,
	PointBlankShot, PowerAttack, PreciseShoot,
	RapidShot, RapidReload,
	ShortOnTheRun,
	SpellFocus,
	SpringAttack, StunningFist,
	Toughness, Track, TwoWeaponFighting,
	FocusAxes, FocusBows, FocusDaggers,
	FocusMaces, FocusPolearm, FocusSwords, FocusGreatSwords,
	WhirlwindAttack,
	FastMovement, Illiteracy,
	HateGoblinoids, Infravision, BonusFeat, BonusFighterFeat, BonusSkills,
	ReadyToBattle,
};
struct feati : nameable {
	unsigned flags;
	featn upgrade;
	variants require;
	featn getindex() const;
	bool is(featgf v) const { return (flags & FG(v)) != 0; }
};
typedef flagable<(ReadyToBattle + 31) / 32, unsigned> featf;

void initialize_feats();