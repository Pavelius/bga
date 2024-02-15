#include "actor.h"
#include "area.h"
#include "crt.h"

enum animatef : unsigned char {
	DisableOverlay, ReadyStance
};

struct animatei : nameable {
	unsigned	flags;
	bool		is(animatef v) const { return FGT(flags, v); }
};
BSDATA(animatei) = {
	{"AnimateMove"},
	{"AnimateStand", FG(ReadyStance)},
	{"AnimateStandRelax", FG(ReadyStance)},
	{"AnimateStandLook", FG(ReadyStance)},
	{"AnimateCombatStance", FG(ReadyStance)},
	{"AnimateCombatStanceTwoHanded", FG(ReadyStance)},
	{"AnimateGetHit"},
	{"AnimateGetHitAndDrop"},
	{"AnimateAgony", FG(ReadyStance)},
	{"AnimateGetUp"},
	{"AnimateMeleeOneHanded"},
	{"AnimateMeleeOneHandedSwing"},
	{"AnimateMeleeOneHandedThrust"},
	{"AnimateMeleeTwoHanded"},
	{"AnimateMeleeTwoHandedSwing"},
	{"AnimateMeleeTwoHandedThrust"},
	{"AnimateMeleeTwoWeapon"},
	{"AnimateMeleeTwoWeaponSwing"},
	{"AnimateMeleeTwoWeaponThrust"},
	{"AnimateShootBow"},
	{"AnimateShootSling"},
	{"AnimateShootXBow"},
	{"AnimateCastBig", FG(DisableOverlay)},
	{"AnimateCastBigRelease", FG(DisableOverlay)},
	{"AnimateCast", FG(DisableOverlay)},
	{"AnimateCastRelease", FG(DisableOverlay)},
	{"AnimateCastThird", FG(DisableOverlay)},
	{"AnimateCastThirdRelease", FG(DisableOverlay)},
	{"AnimateCastFour", FG(DisableOverlay)},
	{"AnimateCastFourRelease", FG(DisableOverlay)},
};
assert_enum(animatei, AnimateCastFourRelease)

bool actor::ispresent() const {
	return area_index == current_area;
}

void actor::clearpath() {
}

void actor::stop() {
	action = AnimateStand;
	if(is(ReadyToBattle)) {
		if(getweapon().geti().is(TwoHanded))
			action = AnimateCombatStanceTwoHanded;
		else
			action = AnimateCombatStance;
	}
	//action_target.clear();
	frame = 0;
	//duration = draw::getframe();
	clearpath();
}

void actor::lookat(point destination) {
	setorientation(map::getorientation(position, destination));
}