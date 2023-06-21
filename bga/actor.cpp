#include "actor.h"
#include "area.h"
#include "crt.h"

struct animatei : nameable {
	bool		disable_overlay;
	bool		ready;
};
BSDATA(animatei) = {
	{"AnimateMove"},
	{"AnimateStand", false, true},
	{"AnimateStandRelax", false, true},
	{"AnimateStandLook", false, true},
	{"AnimateCombatStance", false, true},
	{"AnimateCombatStanceTwoHanded", false, true},
	{"AnimateGetHit"},
	{"AnimateGetHitAndDrop"},
	{"AnimateAgony", false, true},
	{"AnimateGetUp", false},
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
	{"AnimateCastBig", true},
	{"AnimateCastBigRelease", true},
	{"AnimateCast", true},
	{"AnimateCastRelease", true},
	{"AnimateCastThird", true},
	{"AnimateCastThirdRelease", true},
	{"AnimateCastFour", true},
	{"AnimateCastFourRelease", true},
};
assert_enum(animatei, AnimateCastFourRelease)

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