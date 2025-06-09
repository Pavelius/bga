#include "actor.h"
#include "area.h"
#include "bsdata.h"
#include "draw.h"
#include "gender.h"
#include "rand.h"
#include "timer.h"
// #include "ftflag.h"

using namespace draw;

enum animatef : unsigned char {
	DisableOverlay, ReadyStance
};
struct animatei : nameable {
	unsigned	flags;
	bool		is(animatef v) const { return FGT(flags, v); }
};
BSDATA(animatei) = {
	{"AnimateMove", 0},
	{"AnimateStand", FG(ReadyStance)},
	{"AnimateStandRelax", FG(ReadyStance)},
	{"AnimateStandLook", FG(ReadyStance)},
	{"AnimateCombatStance", FG(ReadyStance)},
	{"AnimateCombatStanceTwoHanded", FG(ReadyStance)},
	{"AnimateGetHit"},
	{"AnimateGetHitAndDrop"},
	{"AnimateAgony", FG(ReadyStance)},
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
	{"AnimateCastBig", FG(DisableOverlay)},
	{"AnimateCastBigRelease", FG(DisableOverlay)},
	{"AnimateCast", FG(DisableOverlay)},
	{"AnimateCastRelease", FG(DisableOverlay)},
	{"AnimateCastThird", FG(DisableOverlay)},
	{"AnimateCastThirdRelease", FG(DisableOverlay)},
	{"AnimateCastFour", FG(DisableOverlay)},
	{"AnimateCastFourRelease", FG(DisableOverlay)},
};
assert_enum(animatei, AnimateCastFourRelease)

const int max_sprite_directions = 9;

int get_armor_index(const item& e) {
	auto v = e.geti().required;
	if(v.iskind<abilityi>() && v.value == ArmorProficiency)
		return v.counter;
	return 0;
}

resn get_character_res(racen race, gendern gender, classn type, int ai, int& ws) {
	resn icn = NONE;
	switch(race) {
	case Dwarf:
	case Gnome:
		icn = CDMB1;
		ws = 0;
		break;
	case Elf:
	case HalfElf:
		if(gender == Female)
			icn = CEFB1;
		else
			icn = CEMB1;
		ws = 2;
		break;
	case Halfling:
		if(gender == Female)
			icn = CIFB1;
		else
			icn = CIMB1;
		if(type == Wizard || type == Sorcerer)
			type = Rogue;
		if(ai > 1)
			ai = 1;
		ws = 0;
		break;
	default:
		if(gender == Female) {
			ws = 1;
			icn = CHFB1;
		} else {
			ws = 3;
			icn = CHMB1;
		}
		break;
	}
	if(type == Wizard || type == Sorcerer)
		icn = (resn)(icn + (CDMW1 - CDMB1) + ai);
	else if(type == Cleric)
		icn = (resn)(icn + ai);
	else if(type == Rogue && ai)
		icn = (resn)(icn + (CDMT1 - CDMB1));
	else {
		if(ai == 3)
			icn = (resn)(icn + 4);
		else
			icn = (resn)(icn + ai);
	}
	return icn;
}

bool actor::ispresent() const {
	return area_index == current_area;
}

static animaten common(animaten v) {
	switch(v) {
	case AnimateStand: case AnimateCombatStanceTwoHanded: case AnimateCombatStance:
		return AnimateStand;
	default:
		return v;
	}
}

sprite* actor::getsprite(int& ws) const {
	if(npc == 0xFFFF)
		return gres(get_character_res(race, gender, getmainclass(), get_armor_index(wears[Body]), ws));
	else
		return 0;
}

void actor::wait(unsigned milliseconds) {
	time_next = current_game_tick + milliseconds;
}

static sprite::cicle* get_cicle(sprite* ps, animaten action, int o) {
	if(o >= max_sprite_directions)
		o = (max_sprite_directions - 1) * 2 - o;
	return ps->gcicle(action * max_sprite_directions + o);
}

static unsigned get_flags(int o) {
	if(o >= max_sprite_directions)
		return ImageMirrorH;
	else
		return 0;
}

void actor::resetframes() {
	sprite* ps = getsprite();
	if(!ps)
		return;
	auto ff = get_flags(orientation);
	auto pc = get_cicle(ps, action, orientation);
	if(frame >= pc->start && frame < (pc->start + pc->count) && ff == frame_flags)
		return;
	frame_flags = ff;
	frame_start = pc->start;
	frame_stop = frame_start + pc->count - 1;
	frame_flags = get_flags(orientation);
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
	wait(xrand(0, 500));
}

void actor::lookat(point destination) {
	setorientation(map::getorientation(position, destination));
	resetframes();
}

void actor::moveto(point destination) {
	if(destination == position)
		return;
	auto new_position = map::getfree(destination, 1);
	area_index = current_area;
	lookat(new_position);
	setposition(new_position);
	stop();
}

unsigned actor::getwait() const {
	return 100;
}

void actor::nextaction() {
	resetaction();
	switch(action) {
	case AnimateStand:
		if(chance(10))
			setanimate(chance(50) ? AnimateStandLook : AnimateStandRelax);
		break;
	case AnimateStandLook:
	case AnimateStandRelax:
		setanimate(AnimateStand);
		break;
	default:
		break;
	}
}

void actor::updateanimate() {
	if(frame == frame_stop)
		nextaction();
	else if(frame < frame_stop)
		frame++;
	else
		frame--;
	wait(getwait());
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
	source = gres(get_character_res(race, gender, type, get_armor_index(armor), ws));
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

void actor::paint() const {
	int ws;
	auto ps = getsprite(ws);
	if(!ps)
		return;
	color pallette[256]; setpallette(pallette);
	apply_shadow(pallette, map::getshadow(position));
	image(ps, frame, frame_flags, pallette);
	painting_equipment(getweapon(), ws, frame, frame_flags, pallette);
	painting_equipment(wears[Head], ws, frame, frame_flags, pallette);
	painting_equipment(getoffhand(), ws, frame, frame_flags, pallette);
}
