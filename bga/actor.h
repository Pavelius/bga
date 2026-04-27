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

struct actor : moveable, coloration, statable, classa, wearable {
	racen			race;
	gendern			gender;
	short unsigned	resid[4]; // Images overlays. 0xFFFF is none.
	short unsigned	cicle;
	short unsigned	position_index;
	animaten		action;
	short unsigned	area_index;
	short unsigned	frame, frame_stop, frame_start;
	unsigned		frame_flags;
	unsigned char	orientation;
	int				delay;
	short unsigned	speak;
	featf			feats;
	void			animateattack(drawable* target);
	void			animatedamage();
	rect			getbox() const;
	point			getlu() const;
	int				getmovement() const { return 6; }
	int				getspeed() const { return 7; }
	int				getsize() const { return 1; }
	bool			ispresent() const;
	void			lookat(point destination);
	void			lookat(directionn direction);
	void			moveto(point destination);
	void			paint() const;
	void			readybattle(bool v);
	void			setorientation(unsigned char v) { orientation = v; }
	void			setposition(point v);
	void			stop();
	void			updateanimate();
private:
	sprite*			getsprite() const;
	unsigned		getwait() const;
	void			nextaction();
	void			resetaction() { frame = frame_start; }
	void			resetframes();
	void			setanimate(animaten v) { action = v; resetframes(); }
	void			setreverse(animaten v);
	void			wait(unsigned milliseconds);
};

int get_armor_index(const item& e);

void paperdoll(const coloration& colors, short unsigned* resid, int animation, int orientation, int frame_tick);
void set_resid(short unsigned* resid, racen race, gendern gender, classn type, int ai, const item& weapon, const item& offhand, const item& helm);