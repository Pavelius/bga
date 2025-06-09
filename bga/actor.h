#include "ability.h"
#include "class.h"
#include "coloration.h"
#include "drawable.h"
#include "npc.h"
#include "race.h"
#include "wearable.h"

#pragma once

struct sprite;

enum animaten : unsigned char {
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
	AnimateCastBig, AnimateCastBigRelease,
	AnimateCast, AnimateCastRelease,
	AnimateCastThird, AnimateCastThirdRelease,
	AnimateCastFour, AnimateCastFourRelease
};
struct actor : drawable, coloration, statable, classa, wearable {
	point			move_postiion;
	animaten		action;
	short unsigned	area_index;
	short unsigned	frame, frame_stop, frame_start;
	unsigned		frame_flags;
	unsigned char	orientation;
	unsigned long	time_next;
	unsigned short	npc;
	racen			race;
	gendern			gender;
	sprite*			getsprite(int& ws) const;
	sprite*			getsprite() const { int ws; return getsprite(ws); }
	unsigned		getwait() const;
	bool			ispresent() const;
	void			lookat(point destination);
	void			moveto(point destination);
	void			paint() const;
	void			setanimate(animaten v);
	void			setorientation(unsigned char v) { orientation = v; }
	void			stop();
	void			updateanimate();
	void			wait(unsigned milliseconds);
private:
	void			checkframes();
};

int get_armor_index(const item& e);

resn get_character_res(racen race, gendern gender, classn type, int ai, int& ws);