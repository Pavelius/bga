#include "flagable.h"
#include "nameable.h"
#include "variant.h"

#pragma once

enum feat_s : unsigned char {
	Alertness,
	BlindFight,
	Cleave, CombatCasting, CombatExpertise, CombatReflexes,
	DeadlyPercision, DeflectsArrows, Dodge, Endurance,
	FarShoot,
	GreateFortitude,
	ImprovedCritical, ImprovedDisarm, ImprovedFeint, ImprovedInitiative, ImprovedOverrun, ImprovedSunder, ImprovedTrip, ImprovedUnarmedStrike,
	IronWill,
	Leadership, LightingReflexes,
	Mobiliy, Multiattack,
	PointBlankShoot, PowerAttack, PreciseShoot,
	RapidShoot, RapidReload,
	ShieldProficiency,
	ShortOnTheRun,
	SpellFocus,
	SpringAttack, StunningFist,
	Toughness, Track, TwoWeaponFighting,
	FocusAxes, FocusDaggers,
	FocusMaces, FocusPolearm, FocusShooting, FocusSwords,
	WhirlwindAttack,
	FastMovement, Illiteracy,
	HateGoblinoids, Infravision, ImmunityToSleepSpell, FindSecretDoors, Lucky, PreciseThrower,
	SaveBonusVsEnchantment, SaveBonusVsFear, SaveBonusVsIllusion, SaveBonusVsPoison, SaveBonusVsSpell, Stability, Stonecunning,
	ReadyToBattle,
};
struct feati : nameable {
	variants require;
};
typedef flagable<(ReadyToBattle + 7) / 8> featf;