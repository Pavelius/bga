#include "actor.h"
#include "area.h"
#include "bsdata.h"
#include "draw.h"
#include "gender.h"
#include "math.h"
#include "npc.h"
#include "order.h"
#include "rand.h"
#include "rfiles.h"
#include "timer.h"

using namespace draw;

struct animatei : nameable {
	int a1o8;
};
BSDATA(animatei) = {
	{"AnimateMove", 9},
	{"AnimateStand", 0},
	{"AnimateStandRelax", 0},
	{"AnimateStandLook", 0},
	{"AnimateCombatStance", 1},
	{"AnimateCombatStanceTwoHanded", 1},
	{"AnimateGetHit", 2},
	{"AnimateGetHitAndDrop", 3},
	{"AnimateAgony", 4},
	{"AnimateGetUp", 5},
	{"AnimateMeleeOneHanded", 10},
	{"AnimateMeleeOneHandedSwing", 11},
	{"AnimateMeleeOneHandedThrust", 12},
	{"AnimateMeleeTwoHanded", 10},
	{"AnimateMeleeTwoHandedSwing", 11},
	{"AnimateMeleeTwoHandedThrust", 12},
	{"AnimateMeleeTwoWeapon", 10},
	{"AnimateMeleeTwoWeaponSwing", 11},
	{"AnimateMeleeTwoWeaponThrust", 12},
	{"AnimateShootBow", 10},
	{"AnimateShootSling", 10},
	{"AnimateShootXBow", 10},
	{"AnimateCastBig", 6},
	{"AnimateCastBigRelease", 7},
	{"AnimateCast", 6},
	{"AnimateCastRelease", 7},
	{"AnimateCastThird", 6},
	{"AnimateCastThirdRelease", 7},
	{"AnimateCastFour", 6},
	{"AnimateCastFourRelease", 7},
};
assert_enum(animatei, AnimateCastFourRelease)

const int max_sprite_directions = 9;
const int anm_monsters_a1 = 104;

int get_armor_index(const item& e) {
	switch(e.geti().required) {
	case ArmorProficiencyLight: return 1;
	case ArmorProficiencyMedium: return 2;
	case ArmorProficiencyHeavy: return 3;
	default: return 0;
	}
}

sprite* get_character_res(racen race, gendern gender, classn type, int ai, int& ws) {
	// `ai` is armor index
	// 0 - no armor
	// 1 - light (or leather) armor
	// 2 - medium armor (chaimail and other)
	// 3 - heavy armor (plate and like)
	// `ws` is index of weapon set
	auto& ei = bsdata<racei>::elements[race];
	auto i = (gender == Female) ? 1 : 0;
	auto p = ei.res[i];
	if(!p)
		return 0;
	auto& ec = bsdata<classi>::elements[type];
	auto n = ec.ai;
	// Class of animation.
	// 0 - default
	// 1 - cleric like
	// 2 - theif like
	// 3 - wizard like
	// 4 - monk like
	ws = ei.ws[i];
	// Allowed animation set (by count of animation types)
	// 10 - CDMB1, CDMB2, CDMB3, CDMC4, CDMF4, CDMT1, CDMW1, CDMW2, CDMW3, CDMW4
	// 11 - CHFB1, CHFB2, CHFB3, CHFC4, CHFF4, CHFM1, CHFT1, CHFW1, CHFW2, CHFW3, CHFW4,
	//  6 - CIMB1, CIMB2, CIMB3, CIMC4, CIMF4, CIMT1
	if(n==0 && ai == 3)
		ai = 4; // Default animation have other index for heavy armor
	switch(ei.resm) {
	case 10:
		switch(n) {
		case 2: case 4: p += 5; break;
		case 3: p += 6 + ai; break;
		default: p += ai; break;
		}
		break;
	case 6:
		switch(n) {
		case 2: case 3: case 4: p += 5; break;
		default: p += ai; break;
		}
		break;
	default:
		switch(n) {
		case 2: p += 6; break;
		case 3: p += 7 + ai; break;
		case 4: p += 5; break;
		default: p += ai; break;
		}
		break;
	}
	return p->get();
}

bool actor::ispresent() const {
	return area_index == current_area;
}

//static animaten common(animaten v) {
//	switch(v) {
//	case AnimateStand: case AnimateCombatStanceTwoHanded: case AnimateCombatStance:
//		return AnimateStand;
//	default:
//		return v;
//	}
//}

npci* actor::getnpc() const {
	return (npc == 0xFFFF) ? 0 : bsdata<npci>::elements + npc;
}

sprite* actor::getsprite(int& ws) const {
	auto npc = getnpc();
	if(!npc)
		return get_character_res(race, gender, getmainclass(), get_armor_index(wears[Body]), ws);
	else
		return npc->getres(0);
}

void actor::wait(unsigned milliseconds) {
	delay += milliseconds;
	// time_next = current_game_tick + milliseconds;
}

static sprite::cicle* get_cicle(sprite* ps, animaten action, int o) {
	if(ps->cicles== anm_monsters_a1)
		return ps->gcicle(bsdata<animatei>::elements[action].a1o8 * 8 + o / 2);
	else {
		// Standat character animation
		if(o >= max_sprite_directions)
			o = (max_sprite_directions - 1) * 2 - o;
		return ps->gcicle(action * max_sprite_directions + o);
	}
}

static unsigned get_flags(sprite* ps, int o) {
	if(ps->cicles == anm_monsters_a1)
		return 0;
	if(o >= max_sprite_directions)
		return ImageMirrorH;
	else
		return 0;
}

void actor::resetframes() {
	sprite* ps = getsprite();
	if(!ps)
		return;
	auto ff = get_flags(ps, orientation);
	auto pc = get_cicle(ps, action, orientation);
	auto fb = pc->start;
	auto fe = pc->start + pc->count;
	if(frame >= fb && frame < fe && frame_start == fb && frame_stop == fe && ff == frame_flags)
		return;
	frame_flags = ff;
	frame_start = pc->start;
	frame_stop = frame_start + pc->count - 1;
	frame = frame_start;
}

void actor::stop() {
	if(is(ReadyToBattle)) {
		if(getweapon().geti().is(TwoHanded))
			setanimate(AnimateCombatStanceTwoHanded);
		else
			setanimate(AnimateCombatStance);
	} else
		setanimate(AnimateStand);
}

void actor::lookat(point destination) {
	lookat((directionn)get_look(position, destination));
}

void actor::lookat(directionn direction) {
	setorientation(direction);
	resetframes();
}

void actor::moveto(point destination) {
	if(destination == position)
		return;
	area_index = current_area;
	lookat(destination);
	//setposition(new_position);
	//stop();
	setanimate(AnimateMove);
	move_start = position;
	move_stop = destination;
}

rect actor::getbox() const {
	auto s = getsize();
	return {position.x - s * 8, position.y - s * 6, position.x + s * 8, position.y + s * 6};
}

unsigned actor::getwait() const {
	return 74;
}

void actor::setreverse(animaten v) {
	setanimate(v);
	iswap(frame_start, frame_stop);
	frame = frame_start;
}

void actor::nextaction() {
	resetaction();
	switch(action) {
	case AnimateStand:
		if(chance(10))
			setanimate(chance(50) ? AnimateStandLook : AnimateStandRelax);
		break;
	case AnimateCastFour:
		setanimate(AnimateCastFourRelease);
		break;
	case AnimateCastThird:
		setanimate(AnimateCastRelease);
		break;
	case AnimateCast:
		setanimate(AnimateCastThirdRelease);
		break;
	case AnimateGetHitAndDrop:
		wait(xrand(300, 1200));
		setanimate(AnimateAgony);
		break;
	case AnimateAgony:
		if(chance(20))
			setreverse(AnimateGetUp);
		else
			wait(xrand(1000, 10000));
		break;
	case AnimateMeleeOneHanded: case AnimateMeleeOneHandedSwing: case AnimateMeleeOneHandedThrust:
	case AnimateMeleeTwoHanded: case AnimateMeleeTwoHandedSwing: case AnimateMeleeTwoHandedThrust:
	case AnimateMeleeTwoWeapon: case AnimateMeleeTwoWeaponSwing: case AnimateMeleeTwoWeaponThrust:
	case AnimateShootBow: case AnimateShootSling: case AnimateShootXBow:
	case AnimateGetUp:
	case AnimateCastFourRelease:
	case AnimateCastThirdRelease:
	case AnimateCastRelease:
	case AnimateStandLook:
	case AnimateStandRelax:
	case AnimateGetHit:
		stop();
		break;
	default:
		break;
	}
}

void actor::updateanimate() {
	delay -= current_tick_delta;
	while(delay < 0) {
		auto prev_action = action;
		wait(getwait());
		if(frame == frame_stop)
			nextaction();
		else if(frame < frame_stop)
			frame++;
		else
			frame--;
		if(action == AnimateMove) {
			movestep(getspeed());
			if(!ismoving())
				stop();
		}
		if(prev_action != action)
			activate_order(this);
	}
}

static void painting_equipment(item equipment, int ws, int frame, unsigned flags, color* pallette) {
	if(!equipment)
		return;
	auto tb = equipment.geti().equiped;
	if(tb)
		image(gres(resn(tb + ws)), frame, flags, pallette);
}

static void apply_shadow(color* pallette, color fore) {
	for(auto i = 0; i < 256; i++)
		pallette[i] = pallette[i] * fore;
}

void paperdoll(color* pallette, racen race, gendern gender, classn type, int animation, int orientation, int frame_tick, const item& armor, const item& weapon, const item& offhand, const item& helm) {
	sprite* source;
	unsigned flags;
	int ws;
	source = get_character_res(race, gender, type, get_armor_index(armor), ws);
	if(!source)
		return;
	const int directions = 9;
	int o = orientation;
	if(o >= directions) {
		flags = ImageMirrorH;
		o = (9 - 1) * 2 - o;
	} else
		flags = 0;
	auto frame = source->ganim(animation * directions + o, frame_tick);
	image(source, frame, flags, pallette);
	painting_equipment(weapon, ws, frame, flags, pallette);
	painting_equipment(helm, ws, frame, flags, pallette);
	painting_equipment(offhand, ws, frame, flags, pallette);
}

void paperdoll(const coloration& colors, racen race, gendern gender, classn type, int animation, int orientation, int frame_tick, const item& armor, const item& weapon, const item& offhand, const item& helm) {
	color pallette[256]; colors.setpallette(pallette);
	paperdoll(pallette, race, gender, type, animation, orientation, frame_tick, armor, weapon, offhand, helm);
}

point actor::getlu() const {
	return a2s(position, getsize());
}

void actor::paint() const {
	int ws;
	auto ps = getsprite(ws);
	if(!ps)
		return;
	if(ps->cicles == anm_monsters_a1)
		image(ps, frame, frame_flags);
	else {
		color pallette[256]; setpallette(pallette);
		apply_shadow(pallette, get_shadow(getlu()));
		image(ps, frame, frame_flags, pallette);
		painting_equipment(getweapon(), ws, frame, frame_flags, pallette);
		painting_equipment(wears[Head], ws, frame, frame_flags, pallette);
		painting_equipment(getoffhand(), ws, frame, frame_flags, pallette);
	}
}

void actor::readybattle(bool v) {
	feats.set(ReadyToBattle, v);
	stop();
}

void actor::animateattack(drawable* target) {
	lookat(target->position);
	auto& w = getweapon();
	auto n = xrand(0, 2);
	if(getoffhand().isweapon())
		setanimate((animaten)(AnimateMeleeTwoWeapon + n));
	else if(w.is(TwoHanded))
		setanimate((animaten)(AnimateMeleeTwoHanded + n));
	else
		setanimate((animaten)(AnimateMeleeOneHanded + n));
}

void actor::animatedamage() {
	setanimate(AnimateGetHit);
}

void actor::setposition(point v) {
	position = v;
	position_index = s2i(a2s(v, getsize()));
}