#include "actor.h"
#include "area.h"
#include "crt.h"
#include "ftflag.h"

enum animatef : unsigned char {
	DisableOverlay, ReadyStance
};

struct animatei : nameable {
	unsigned	flags;
	bool		is(animatef v) const { return FGT(flags, v); }
};
BSDATA(animatei) = {
	{"AnimateMove"},
	{"AnimateStand", fg(ReadyStance)},
	{"AnimateStandRelax", fg(ReadyStance)},
	{"AnimateStandLook", fg(ReadyStance)},
	{"AnimateCombatStance", fg(ReadyStance)},
	{"AnimateCombatStanceTwoHanded", fg(ReadyStance)},
	{"AnimateGetHit"},
	{"AnimateGetHitAndDrop"},
	{"AnimateAgony", fg(ReadyStance)},
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
	{"AnimateCastBig", fg(DisableOverlay)},
	{"AnimateCastBigRelease", fg(DisableOverlay)},
	{"AnimateCast", fg(DisableOverlay)},
	{"AnimateCastRelease", fg(DisableOverlay)},
	{"AnimateCastThird", fg(DisableOverlay)},
	{"AnimateCastThirdRelease", fg(DisableOverlay)},
	{"AnimateCastFour", fg(DisableOverlay)},
	{"AnimateCastFourRelease", fg(DisableOverlay)},
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

void actor::moveto(point destination) {
	if(destination == position)
		return;
	auto new_position = map::getfree(destination, 1);
	area_index = current_area;
	lookat(new_position);
	setposition(new_position);
}