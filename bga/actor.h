#include "ability.h"
#include "actable.h"
#include "class.h"
#include "coloration.h"
#include "drawable.h"
#include "race.h"
#include "wearable.h"

#pragma once

enum animate_s : unsigned char {
	AnimateMove,
	AnimateStand, AnimateStandRelax, AnimateStandLook,
	AnimateCombatStance, AnimateCombatStanceTwoHanded,
	AnimateGetHit,
	AnimateGetHitAndDrop, AnimateAgony,
	AnimateGetUp,
	AnimateMeleeOneHanded, AnimateMeleeOneHandedSwing, AnimateMeleeOneHandedThrust,
	AnimateMeleeTwoHanded, AnimateMeleeTwoHandedSwing, AnimateMeleeTwoHandedThrust,
	AnimateMeleeTwoWeapon, AnimateMeleeTwoWeaponSwing, AnimateMeleeTwoWeaponThrust,
	AnimateShootBow, AnimateShootSling, AnimateShootXBow,
	AnimateCastBig, AnimateCastBigRelease, AnimateCast, AnimateCastRelease, AnimateCastThird, AnimateCastThirdRelease, AnimateCastFour, AnimateCastFourRelease
};
struct actor : drawable, actable, coloration, statable, classa, wearable {
	animate_s		action;
	short unsigned	area_index;
	short unsigned	frame;
	unsigned char	orientation;
	unsigned		duration;
	race_s			race;
	void			clearpath();
	bool			ispresent() const;
	void			lookat(point destination);
	void			setorientation(unsigned char v) { orientation = v; }
	void			stop();
};
