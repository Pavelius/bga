#include "flagable.h"
#include "nameable.h"
#include "variant.h"

#pragma once

enum featgf : unsigned char {
	GeneralFeat, FighterFeat, UpgradeFeat
};
enum featn : unsigned char {
	Enemy,
	Alertness,
	ArmorProficiencyLight,
	ArmorProficiencyMedium,
	ArmorProficiencyHeavy,
	BlindFight,
	Cleave, CombatCasting, CombatExpertise, CombatReflexes,
	DeadlyPercision, DeflectArrows, Dodge, Endurance,
	FarShoot,
	GreateFortitude,
	ImprovedCritical, ImprovedDisarm, ImprovedFeint, ImprovedInitiative, ImprovedOverrun, ImprovedSunder, ImprovedTrip, ImprovedUnarmedStrike,
	IronWill,
	Leadership, LightingReflexes,
	Mobiliy, Multiattack,
	PointBlankShoot, PowerAttack, PreciseShoot,
	RapidShoot, RapidReload,
	SimpleWeaponMace, SimpleWeaponMissile, SimpleWeaponStaff, SimpleWeaponDagger,
	MartialWeaponAxe, MartialWeaponBow, MartialWeaponFlail, MartialWeaponGreatSword, MartialWeaponHammer,
	MartialWeaponSword, MartialWeaponPolearm,
	ShieldProficiency,
	ShortOnTheRun,
	SpellFocus,
	SpringAttack, StunningFist,
	Toughness, Track, TwoWeaponFighting,
	FocusAxes, FocusBow, FocusDaggers,
	FocusMaces, FocusPolearm, FocusSwords, FocusGreatSword,
	WhirlwindAttack,
	FastMovement, Illiteracy,
	HateGoblinoids, Infravision, BonusFeat, BonusSkills, 
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