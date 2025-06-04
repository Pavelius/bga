#include "animation.h"
#include "console.h"
#include "creature.h"
#include "draw.h"
#include "draw_focus.h"
#include "drawable.h"
#include "game.h"
#include "map.h"
#include "math.h"
#include "resid.h"
#include "timer.h"
#include "view.h"

using namespace draw;

static int current_info_tab;
static bool need_update;

void update_creature() {
	if(player)
		player->update();
}

static void update_creature_info() {
	//if(!need_update)
	//	return;
	//need_update = false;
	//description.clear();
	//switch(current_info_tab) {
	//case 0: player->getinfo(description); break;
	//case 2: player->getskillsinfo(description); break;
	//default: break;
	//}
}

static void update_help_info() {
	//if(!need_update)
	//	return;
	//need_update = false;
	//variant v = current_focus;
	//description.clear();
	//auto pn = getnme(ids(v.getid(), "Info"));
}

void invalidate_description() {
	need_update = true;
}

static resn getanimation(racen race, gendern gender, classn type, int ai, int& ws) {
	resn icn;
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

static int getarmorindex(const item& e) {
	auto v = e.geti().required;
	if(v.iskind<abilityi>() && v.value == ArmorProficiency)
		return v.counter;
	return 0;
}

static void painting_equipment(item equipment, int ws, int frame, unsigned flags, color* pallette) {
	if(!equipment)
		return;
	auto tb = equipment.geti().equiped;
	if(tb)
		image(gres(resn(tb + ws)), frame, flags, pallette);
}

void paperdoll(color* pallette, racen race, gendern gender, classn type, int animation, int orientation, int frame_tick, const item& armor, const item& weapon, const item& offhand, const item& helm) {
	sprite* source;
	unsigned flags;
	int ws;
	source = gres(getanimation(race, gender, type, getarmorindex(armor), ws));
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

static void paperdoll(const coloration& colors, racen race, gendern gender, classn type, int animation, int orientation, int frame_tick, const item& armor, const item& weapon, const item& offhand, const item& helm) {
	color pallette[256]; colors.setpallette(pallette);
	paperdoll(pallette, race, gender, type, animation, orientation, frame_tick, armor, weapon, offhand, helm);
}

void paperdoll() {
	static int orientation = 1;
	auto push_caret = caret;
	caret.x += width / 2;
	caret.y += height / 2 + 20;
	paperdoll(*player,
		player->race, player->gender, player->getmainclass(), 1, orientation, current_tick / 100,
		player->wears[Body], player->getweapon(), player->getoffhand(), player->wears[Head]);
	caret = push_caret;
	switch(hot.key) {
	case KeyLeft:
		execute(cbsetint, (orientation >= 15) ? 0 : orientation + 1, 0, &orientation);
		break;
	case KeyRight:
		execute(cbsetint, (orientation <= 0) ? 15 : orientation - 1, 0, &orientation);
		break;
	}
}