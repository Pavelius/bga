#include "actor.h"
#include "area.h"
#include "bsdata.h"
#include "draw.h"
#include "gender.h"
#include "math.h"
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

static int get_cicle(sprite* ps, animaten action, int o) {
	if(!ps)
		return 0;
	switch(ps->cicles) {
	case anm_a1o8:
		return bsdata<animatei>::elements[action].a1o8 * 8 + o / 2;
	case anm_hg1o8:
		return bsdata<animatei>::elements[action].hg1o8 * 8 + o / 2;
	default:
		// Standat character animation
		if(o >= max_sprite_directions)
			o = (max_sprite_directions - 1) * 2 - o;
		return action * max_sprite_directions + o;
	}
}

static unsigned get_flags(sprite* ps, int o) {
	if(!ps)
		return 0;
	switch(ps->cicles) {
	case anm_a1o8: case anm_hg1o8:
		return 0;
	default:
		return (o >= max_sprite_directions) ? ImageMirrorH : 0;
	}
}

bool actor::ispresent() const {
	return area_index == current_area;
}

sprite* actor::getsprite() const {
	if(resid[0] == 0xFFFF)
		return 0;
	return bsdata<rfpma>::get(resid[0]).get();
}

void actor::updateframe() {
	cicle = get_cicle(getsprite(), action, orientation);
	frame = 0;
}

void actor::lookat(point destination) {
	lookat((directionn)get_look(position, destination));
}

void actor::lookat(directionn direction) {
	setorientation(direction);
	updateframe();
}

void actor::moveto(point destination) {
	if(destination == position)
		return;
	lookat(destination);
	set(AnimateMove);
	area_index = current_area;
	move_start = position;
	move_stop = destination;
}

rect actor::getbox() const {
	auto s = getsize();
	return {position.x - s * 8, position.y - s * 6, position.x + s * 8, position.y + s * 6};
}

rect actor::getrect() const {
	auto ps = getsprite();
	if(!ps)
		return rect();
	return ps->get(ps->ganim(cicle, frame)).getrect(position.x, position.y, get_flags(ps, orientation));
}

static void apply_shadow(color* pallette, color fore) {
	for(auto i = 0; i < 256; i++)
		pallette[i] = pallette[i] * fore;
}

unsigned actor::getflags() const {
	return get_flags(getsprite(), orientation);
}

void actor::paint(bool use_colors) const {
	pushvalue push(palt);
	auto flags = getflags();
	if(use_colors) {
		palt = pallette;
		setpallette(pallette);
		apply_shadow(pallette, get_shadow(position));
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

void actor::setposition(point v) {
	position = v;
	position_index = s2i(v);
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