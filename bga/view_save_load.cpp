#include "draw.h"
#include "resid.h"
#include "saveheader.h"
#include "slice.h"
#include "view.h"

using namespace draw;

void get_save_screenshoot(surface& sm);
void get_player_portrait(surface& sm, int index);

void saveheaderi::clear() {
	memset(this, 0, sizeof(*this));
}

static void copy_bitmap(color* p, const surface& sm) {
	memcpy(p, sm.bits, sm.width*sm.height*sizeof(color));
}

void saveheaderi::create() {
	clear();
	surface sm(102, 77, 32); get_save_screenshoot(sm); copy_bitmap(&screenshoot[0][0], sm);
	surface pm(22, 20, 32);
	for(auto i = 0; i < 6; i++) {
		get_player_portrait(pm, i);
		copy_bitmap(&party[i][0][0], pm);
	}
}

static void paint_save_game() {
	paint_game_dialog(GUISRSVB);
	setdialog(243, 22, 311, 28); texta(STONEBIG, getnm("SaveGame"), AlignCenterCenter);
	setdialog(159, 93, 345, 18); texta(NORMAL, "268435461", AlignCenterCenter);
	setdialog(159, 118, 279, 18); texta(NORMAL, "268435466", AlignCenterCenter);
	setdialog(159, 195, 345, 18); texta(NORMAL, "268435462", AlignCenterCenter);
	setdialog(159, 220, 279, 18); texta(NORMAL, "268435467", AlignCenterCenter);
	setdialog(159, 297, 345, 18); texta(NORMAL, "268435463", AlignCenterCenter);
	setdialog(159, 322, 279, 18); texta(NORMAL, "268435468", AlignCenterCenter);
	setdialog(159, 399, 345, 18); texta(NORMAL, "268435464", AlignCenterCenter);
	setdialog(159, 424, 279, 18); texta(NORMAL, "268435469", AlignCenterCenter);
	//Scroll GBTNSCRL 763 78 12 498 frames(1 0 3 2 4 5)
	setdialog(627, 89); button(GBTNSTD, 1, 2);
	setdialog(627, 191); button(GBTNSTD, 1, 2);
	setdialog(627, 293); button(GBTNSTD, 1, 2);
	setdialog(627, 395); button(GBTNSTD, 1, 2);
	setdialog(627, 130); button(GBTNSTD, 1, 2);
	setdialog(627, 233); button(GBTNSTD, 1, 2);
	setdialog(627, 334); button(GBTNSTD, 1, 2);
	setdialog(627, 436); button(GBTNSTD, 1, 2);
	setdialog(656, 22); button(GBTNSTD, 1, 2, KeyEscape, "Cancel"); fire(buttoncancel);
}

void open_save_game() {
	scene(paint_save_game);
}