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

using namespace draw;

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