#include "bsdata.h"
#include "feat.h"

BSDATA(feati) = {
	{"Enemy"},
	{"Alertness", FG(GeneralFeat)},
	{"ArmorProficiencyLight", FG(GeneralFeat), ArmorProficiencyMedium},
	{"ArmorProficiencyMedium", FG(GeneralFeat), ArmorProficiencyHeavy},
	{"ArmorProficiencyHeavy", FG(GeneralFeat)},
	{"BlindFight", FG(GeneralFeat)},
	{"Cleave", FG(GeneralFeat)},
	{"CombatCasting", FG(GeneralFeat)},
	{"CombatExpertise", FG(GeneralFeat)},
	{"CombatReflexes", FG(GeneralFeat)},
	{"DeadlyPercision", FG(GeneralFeat)},
	{"DeflectArrows", FG(GeneralFeat)},
	{"Dodge", FG(GeneralFeat)},
	{"Endurance", FG(GeneralFeat)},
	{"FarShoot", FG(GeneralFeat)},
	{"GreateFortitude", FG(GeneralFeat)},
	{"ImprovedCritical", FG(GeneralFeat)},
	{"ImprovedDisarm", FG(GeneralFeat)},
	{"ImprovedFeint", FG(GeneralFeat)},
	{"ImprovedInitiative", FG(GeneralFeat)},
	{"ImprovedOverrun", FG(GeneralFeat)},
	{"ImprovedSunder", FG(GeneralFeat)},
	{"ImprovedTrip", FG(GeneralFeat)},
	{"ImprovedUnarmedStrike", FG(GeneralFeat)},
	{"IronWill", FG(GeneralFeat)},
	{"Leadership", 0},
	{"LightingReflexes", FG(GeneralFeat)},
	{"Mobiliy", FG(GeneralFeat)},
	{"Multiattack", 0},
	{"PointBlankShoot", FG(GeneralFeat)},
	{"PowerAttack", FG(GeneralFeat)},
	{"PreciseShoot", FG(GeneralFeat)},
	{"RapidShoot", FG(GeneralFeat)},
	{"RapidReload", FG(GeneralFeat)},
	{"SimpleWeaponMace", FG(GeneralFeat), FocusMaces},
	{"SimpleWeaponMissile", FG(GeneralFeat)},
	{"SimpleWeaponStaff", FG(GeneralFeat)},
	{"SimpleWeaponDagger", FG(GeneralFeat), FocusDaggers},
	{"MartialWeaponAxe", FG(GeneralFeat), FocusAxes},
	{"MartialWeaponBow", FG(GeneralFeat), FocusBow},
	{"MartialWeaponFlail", FG(GeneralFeat)},
	{"MartialWeaponGreatSword", FG(GeneralFeat), FocusGreatSword},
	{"MartialWeaponHammer", FG(GeneralFeat)},
	{"MartialWeaponSword", FG(GeneralFeat), FocusSwords},
	{"MartialWeaponPolearm", FG(GeneralFeat), FocusPolearm},
	{"ShieldProficiency", FG(GeneralFeat)},
	{"ShortOnTheRun", FG(GeneralFeat)},
	{"SpellFocus", FG(GeneralFeat)},
	{"SpringAttack", FG(GeneralFeat)},
	{"StunningFist", FG(GeneralFeat)},
	{"Toughness", FG(GeneralFeat)},
	{"Track", FG(GeneralFeat)},
	{"TwoWeaponFighting", FG(GeneralFeat)},
	{"FocusAxes", FG(GeneralFeat)},
	{"FocusBow", FG(GeneralFeat)},
	{"FocusDaggers", FG(GeneralFeat)},
	{"FocusMaces", FG(GeneralFeat)},
	{"FocusPolearm", FG(GeneralFeat)},
	{"FocusSwords", FG(GeneralFeat)},
	{"FocusGreatSword", FG(GeneralFeat)},
	{"WhirlwindAttack", FG(GeneralFeat)},
	{"FastMovement"},
	{"Illiteracy"},
	{"HateGoblinoids"},
	{"Infravision"},
	{"BonusFeat"},
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