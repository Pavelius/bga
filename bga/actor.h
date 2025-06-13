#include "ability.h"
#include "class.h"
#include "coloration.h"
#include "moveable.h"
#include "race.h"
#include "wearable.h"

#pragma once

struct sprite;
struct npci;

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

struct actor : moveable, coloration, statable, classa, wearable {
	short unsigned	position_index;
	animaten		action;
	short unsigned	area_index;
	short unsigned	frame, frame_stop, frame_start;
	unsigned		frame_flags;
	unsigned char	orientation;
	unsigned long	time_next;
	unsigned short	npc, enemy_id;
	racen			race;
	gendern			gender;
	void			animateattack(drawable* target);
	void			animatedamage();
	rect			getbox() const;
	point			getlu() const;
	int				getmovement() const { return 6; }
	npci*			getnpc() const; // Monster and other template characters.
	int				getspeed() const { return 7; }
	int				getsize() const { return 1; }
	bool			ispresent() const;
	void			lookat(point destination);
	void			moveto(point destination);
	void			paint() const;
	void			readybattle(bool v);
	void			setorientation(unsigned char v) { orientation = v; }
	void			setposition(point v);
	void			stop();
	void			updateanimate();
private:
	sprite*			getsprite(int& ws) const;
	sprite*			getsprite() const { int ws; return getsprite(ws); }
	unsigned		getwait() const;
	void			nextaction();
	void			resetaction() { frame = frame_start; }
	void			resetframes();
	void			setanimate(animaten v) { action = v; resetframes(); }
	void			setreverse(animaten v);
	void			wait(unsigned milliseconds);
};

int get_armor_index(const item& e);

resn get_character_res(racen race, gendern gender, classn type, int ai, int& ws);

void paperdoll(const coloration& colors, racen race, gendern gender, classn type, int animation, int orientation, int frame_tick, const item& armor, const item& weapon, const item& offhand, const item& helm);