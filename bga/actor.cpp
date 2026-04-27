#include "actor.h"
#include "area.h"
#include "bsdata.h"
#include "draw.h"
#include "gender.h"
#include "math.h"
#include "order.h"
#include "pushvalue.h"
#include "rand.h"
#include "rfiles.h"
#include "timer.h"

using namespace draw;

static color pallette[256];

struct animatei : nameable {
	int a1o8, hg1o8, g26o9;
};
BSDATA(animatei) = {
	{"AnimateMove", 9, 1, 2},
	{"AnimateStand", 0, 0, 0},
	{"AnimateStandRelax", 0, 0, 0},
	{"AnimateStandLook", 0, 0, 0},
	{"AnimateCombatStance", 1, 0, 1},
	{"AnimateCombatStanceTwoHanded", 1, 0, 1},
	{"AnimateGetHit", 2, 2, 3},
	{"AnimateGetHitAndDrop", 3, 3, 4},
	{"AnimateAgony", 4, 0, 5},
	{"AnimateGetUp", 5, 0, 0},
	{"AnimateMeleeOneHanded", 10, 0, 6},
	{"AnimateMeleeOneHandedSwing", 11, 0, 7},
	{"AnimateMeleeOneHandedThrust", 12, 0, 8},
	{"AnimateMeleeTwoHanded", 10, 0, 6},
	{"AnimateMeleeTwoHandedSwing", 11, 0, 7},
	{"AnimateMeleeTwoHandedThrust", 12, 0, 8},
	{"AnimateMeleeTwoWeapon", 10, 0, 6},
	{"AnimateMeleeTwoWeaponSwing", 11, 0, 7},
	{"AnimateMeleeTwoWeaponThrust", 12, 0, 8},
	{"AnimateShootBow", 10, 0, 9},
	{"AnimateShootSling", 10, 0, 9},
	{"AnimateShootXBow", 10, 0, 9},
	{"AnimateCastBig", 6, 0, 10},
	{"AnimateCastBigRelease", 7, 0, 11},
	{"AnimateCast", 6, 0, 10},
	{"AnimateCastRelease", 7, 0, 11},
	{"AnimateCastThird", 6, 0, 10},
	{"AnimateCastThirdRelease", 7, 0, 11},
	{"AnimateCastFour", 6, 0, 10},
	{"AnimateCastFourRelease", 7, 0, 11},
};
assert_enum(animatei, AnimateCastFourRelease)

const int max_sprite_directions = 9;

const int anm_hg1o8 = 32;
const int anm_a1o8 = 104;
const int anm_g26o9 = 108;

int get_armor_index(const item& e) {
	switch(e.geti().required) {
	case ArmorProficiencyLight: return 1;
	case ArmorProficiencyMedium: return 2;
	case ArmorProficiencyHeavy: return 3;
	default: return 0;
	}
}

static short unsigned get_resid(item equipment, int ws) {
	static int ai[] = {max_weapon_anim * 3, max_weapon_anim * 2, max_weapon_anim * 1, 0};
	if(!equipment)
		return 0xFFFF;
	auto p = equipment.geti().equiped;
	if(!p)
		return 0xFFFF;
	return getbsi(p + ai[ws]);
}

void set_resid(short unsigned* resid, racen race, gendern gender, classn type, int ai, const item& weapon, const item& offhand, const item& helm) {
	static char animation[][5][4] = {
		{{0, 1, 2, 3}, // 11 animations (Human-like)
		{0, 1, 2, 4},
		{6, 6, 6, 6},
		{7, 8, 9, 10},
		{5, 5, 5, 5}},
		{{0, 1, 2, 3}, // 10 animations (Elf-like)
		{0, 1, 2, 4},
		{5, 5, 5, 5},
		{6, 7, 8, 9},
		{5, 5, 5, 5}},
		{{0, 1, 2, 3}, // 6 animations (Halfling-like)
		{0, 1, 2, 4},
		{5, 5, 5, 5},
		{5, 5, 5, 5},
		{5, 5, 5, 5}},
	};
	auto& ei = bsdata<racei>::elements[race];
	auto i = (gender == Female) ? 1 : 0;
	auto r = ei.res[i];
	if(!r)
		return;
	auto w = ei.ws[i];
	resid[0] = r + animation[ei.ai][bsdata<classi>::elements[type].ai][ai];
	resid[1] = get_resid(weapon, w);
	resid[2] = get_resid(helm, w);
	resid[3] = get_resid(offhand, w);
}

bool actor::ispresent() const {
	return area_index == current_area;
}

sprite* actor::getsprite() const {
	if(resid[0] == 0xFFFF)
		return 0;
	return bsdata<rfpma>::get(resid[0]).get();
}

void actor::wait(unsigned milliseconds) {
	delay += milliseconds;
}

static sprite::cicle* get_cicle(sprite* ps, animaten action, int o) {
	switch(ps->cicles) {
	case anm_a1o8:
		return ps->gcicle(bsdata<animatei>::elements[action].a1o8 * 8 + o / 2);
	case anm_hg1o8:
		return ps->gcicle(bsdata<animatei>::elements[action].hg1o8 * 8 + o / 2);
	default:
		// Standat character animation
		if(o >= max_sprite_directions)
			o = (max_sprite_directions - 1) * 2 - o;
		return ps->gcicle(action * max_sprite_directions + o);
	}
}

static unsigned get_flags(sprite* ps, int o) {
	switch(ps->cicles) {
	case anm_a1o8: case anm_hg1o8:
		return 0;
	default:
		return (o >= max_sprite_directions) ? ImageMirrorH : 0;
	}
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
	if(feats.is(ReadyToBattle)) {
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

static void apply_shadow(color* pallette, color fore) {
	for(auto i = 0; i < 256; i++)
		pallette[i] = pallette[i] * fore;
}

void paperdoll(const coloration& colors, short unsigned* resid, int animation, int orientation, int frame_tick) {
	pushvalue push(palt, pallette);
	colors.setpallette(pallette);
	const int directions = 9;
	int o = orientation;
	unsigned flags = ImagePallette;
	if(o >= directions) {
		flags |= ImageMirrorH;
		o = (9 - 1) * 2 - o;
	}
	for(auto i = 0; i < 4; i++) {
		auto ri = resid[i];
		if(ri == 0xFFFF)
			continue;
		auto ps = bsdata<rfpma>::ptr(ri)->get();
		auto frame = ps->ganim(animation * directions + o, frame_tick);
		image(caret.x, caret.y, ps, frame, flags);
	}
}

point actor::getlu() const {
	return a2s(position, getsize());
}

void actor::paint() const {
	pushvalue push(palt);
	auto flags = frame_flags;
	if(feats.is(DynamicAnimation)) {
		palt = pallette;
		setpallette(pallette);
		apply_shadow(pallette, get_shadow(getlu()));
		flags |= ImagePallette;
	}
	for(auto n : resid) {
		if(n == 0xFFFF)
			continue;
		auto ps = bsdata<rfpma>::ptr(n)->get();
		auto pc = ps->gcicle(cicle);
		image(ps, pc->start + frame, flags);
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