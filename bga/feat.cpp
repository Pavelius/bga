#include "bsdata.h"
#include "feat.h"

BSDATA(feati) = {
	{"Enemy"},
	{"Alertness", FG(GeneralFeat)},
	{"ArmorProficiencyLight", FG(GeneralFeat), ArmorProficiencyMedium},
	{"ArmorProficiencyMedium", FG(GeneralFeat), ArmorProficiencyHeavy},
	{"ArmorProficiencyHeavy", FG(GeneralFeat)},
	{"BlindFight", FG(GeneralFeat) | FG(FighterFeat)},
	{"Cleave", FG(GeneralFeat) | FG(FighterFeat)},
	{"CombatCasting", FG(GeneralFeat)},
	{"CombatExpertise", FG(GeneralFeat) | FG(FighterFeat)},
	{"CombatReflexes", FG(GeneralFeat) | FG(FighterFeat)},
	{"DeadlyPercision", FG(GeneralFeat) | FG(FighterFeat)},
	{"DeflectArrows", FG(GeneralFeat) | FG(FighterFeat)},
	{"Dodge", FG(GeneralFeat) | FG(FighterFeat), Mobility},
	{"Endurance", FG(GeneralFeat)},
	{"FarShoot", FG(GeneralFeat) | FG(FighterFeat)},
	{"GreateFortitude", FG(GeneralFeat)},
	{"ImprovedCritical", FG(GeneralFeat) | FG(FighterFeat)},
	{"ImprovedDisarm", FG(GeneralFeat) | FG(FighterFeat)},
	{"ImprovedFeint", FG(GeneralFeat) | FG(FighterFeat)},
	{"ImprovedInitiative", FG(GeneralFeat) | FG(FighterFeat)},
	{"ImprovedOverrun", FG(GeneralFeat) | FG(FighterFeat)},
	{"ImprovedSunder", FG(GeneralFeat) | FG(FighterFeat)},
	{"ImprovedTrip", FG(GeneralFeat) | FG(FighterFeat)},
	{"ImprovedUnarmedStrike", FG(GeneralFeat) | FG(FighterFeat)},
	{"IronWill", FG(GeneralFeat)},
	{"Leadership", 0},
	{"LightingReflexes", FG(GeneralFeat)},
	{"Mobility", FG(GeneralFeat) | FG(FighterFeat), SpringAttack},
	{"Multiattack", 0},
	{"PointBlankShot", FG(GeneralFeat) | FG(FighterFeat)},
	{"PowerAttack", FG(GeneralFeat) | FG(FighterFeat)},
	{"PreciseShoot", FG(GeneralFeat) | FG(FighterFeat)},
	{"RapidShot", FG(GeneralFeat) | FG(FighterFeat)},
	{"RapidReload", FG(GeneralFeat) | FG(FighterFeat)},
	{"SimpleWeaponMace", FG(GeneralFeat) | FG(FighterFeat), FocusMaces},
	{"SimpleWeaponMissile", FG(GeneralFeat) | FG(FighterFeat)},
	{"SimpleWeaponStaff", FG(GeneralFeat) | FG(FighterFeat)},
	{"SimpleWeaponDagger", FG(GeneralFeat) | FG(FighterFeat), FocusDaggers},
	{"MartialWeaponAxe", FG(GeneralFeat) | FG(FighterFeat), FocusAxes},
	{"MartialWeaponBow", FG(GeneralFeat) | FG(FighterFeat), FocusBow},
	{"MartialWeaponFlail", FG(GeneralFeat) | FG(FighterFeat)},
	{"MartialWeaponGreatSword", FG(GeneralFeat) | FG(FighterFeat), FocusGreatSword},
	{"MartialWeaponHammer", FG(GeneralFeat) | FG(FighterFeat)},
	{"MartialWeaponSword", FG(GeneralFeat) | FG(FighterFeat), FocusSwords},
	{"MartialWeaponPolearm", FG(GeneralFeat) | FG(FighterFeat), FocusPolearm},
	{"ShieldProficiency", FG(GeneralFeat) | FG(FighterFeat)},
	{"ShortOnTheRun", FG(GeneralFeat) | FG(FighterFeat)},
	{"SpellFocus", FG(GeneralFeat)},
	{"SpringAttack", FG(GeneralFeat) | FG(FighterFeat)},
	{"StunningFist", FG(GeneralFeat) | FG(FighterFeat)},
	{"Toughness", FG(GeneralFeat)},
	{"Track", FG(GeneralFeat)},
	{"TwoWeaponFighting", FG(GeneralFeat) | FG(FighterFeat)},
	{"FocusAxes", FG(GeneralFeat) | FG(FighterFeat)},
	{"FocusBow", FG(GeneralFeat) | FG(FighterFeat)},
	{"FocusDaggers", FG(GeneralFeat) | FG(FighterFeat)},
	{"FocusMaces", FG(GeneralFeat) | FG(FighterFeat)},
	{"FocusPolearm", FG(GeneralFeat) | FG(FighterFeat)},
	{"FocusSwords", FG(GeneralFeat) | FG(FighterFeat)},
	{"FocusGreatSword", FG(GeneralFeat) | FG(FighterFeat)},
	{"WhirlwindAttack", FG(GeneralFeat) | FG(FighterFeat)},
	{"FastMovement"},
	{"Illiteracy"},
	{"HateGoblinoids"},
	{"Infravision"},
	{"BonusFeat"},
	{"BonusFighterFeat"},
	{"BonusSkills"},
	{"ReadyToBattle"},
};
assert_enum(feati, ReadyToBattle)

featn feati::getindex() const {
	return (featn)(this - bsdata<feati>::elements);
}

static featn is_require(featn v) {
	for(auto& e : bsdata<feati>()) {
		if(e.upgrade == v)
			return e.getindex();
	}
	return (featn)0;
}

void initialize_feats() {
	for(auto& e : bsdata<feati>()) {
		if(is_require(e.getindex()))
			e.flags |= FG(UpgradeFeat);
	}
}