#include "flagable.h"

#pragma once

enum feat_s : unsigned char {
	Alertness,
	ArmorProfeciencyLight, ArmorProfeciencyMedium, ArmorProfeciencyHeavy,
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
	ShieldProfeciency,
	ShortOnTheRun,
	SpellFocus,
	SpringAttack, StunningFist,
	Toughness, Track, TwoWeaponFighting,
	FocusAxes, FocusDaggers,
	FocusMaces, FocusPolearm, FocusShooting, FocusSwords,
	WhirlwindAttack,
	ProficiencyAxe, ProficiencyClub, ProficiencyCrossbow,
	ProficiencyDagger, ProficiencyGreatweapon, ProficiencyHeavyCrossbow,
	ProficiencyMace, ProficiencySimple, ProficiencySpear,
	ProficiencyLongbow, ProficiencyLongsword,
	ProficiencyScimitar, ProficiencyShortbow, ProficiencyShortsword,
	ProficiencyBastardsword, ProficiencyCatana, ProficiencyWaraxe,
	FastMovement, Illiteracy,
	HateGoblinoids, Infravision, ImmunityToSleepSpell, FindSecretDoors, Lucky, PreciseThrower,
	SaveBonusVsEnchantment, SaveBonusVsFear, SaveBonusVsIllusion, SaveBonusVsPoison, SaveBonusVsSpell, Stability, Stonecunning,
};
struct feati {
	const char* id;
};
typedef flagable<(Stonecunning + 7) / 8> featf;