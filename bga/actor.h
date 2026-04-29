#pragma once

#include "ability.h"
#include "class.h"
#include "coloration.h"
#include "moveable.h"
#include "race.h"
#include "resname.h"
#include "wearable.h"

struct sprite;

const int max_weapon_anim = 26;

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
enum gendern : unsigned char;
enum directionn : unsigned char;

struct actor : moveable, coloration, wearable {
	short unsigned	resid[4]; // Images overlays. 0xFFFF is none.
	short unsigned	cicle; // Current animation cicle
	short unsigned	frame;
	animaten		action;
	unsigned char	orientation;
	short unsigned	area_index;
	int				delay;
	rect			getbox() const;
	unsigned		getflags() const;
	rect			getrect() const;
	int				getspeed() const { return 7; }
	sprite*			getsprite() const;
	int				getmovement() const { return 6; }
	int				getsize() const { return 1; }
	bool			ispresent() const;
	void			lookat(point destination);
	void			lookat(directionn direction);
	void			moveto(point destination);
	void			paint(bool use_colors) const;
	void			set(animaten v) { action = v; updateframe(); }
	void			setorientation(unsigned char v) { orientation = v; }
	void			setposition(point v);
	void			updateframe();
};

int get_armor_index(const item& e);

void paperdoll(const coloration& colors, short unsigned* resid, int animation, int orientation, int frame_tick);
void set_resid(short unsigned* resid, racen race, gendern gender, classn type, int ai, const item& weapon, const item& offhand, const item& helm);