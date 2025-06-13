#include "draw.h"
#include "resid.h"
#include "saveheader.h"
#include "slice.h"
#include "vector.h"
#include "view.h"
#include "view_list.h"

using namespace draw;

static vector<rowsaveheaderi> files;

void get_save_screenshoot(surface& sm);
void get_player_portrait(surface& sm, int index);

void saveheaderi::clear() {
	memset(this, 0, sizeof(*this));
}

void saveheaderi::create() {
	clear();
	surface sm; sm.resize(102, 77, 32, false); sm.bits = (unsigned char*)screenshoot;
	get_save_screenshoot(sm);
	surface pm; pm.resize(20, 22, 32, false);
	for(auto i = 0; i < 6; i++) {
		pm.bits = (unsigned char*)party[i];
		get_player_portrait(pm, i);
	}
	sm.bits = 0;
	pm.bits = 0;
}

static void row_delete() {
}

static void row_save() {
	next_scene(open_game);
}

static void paint_game_row(void* object) {
	pushrect push;
	auto p = (saveheaderi*)object;
	surface sm; sm.resize(102, 77, 32, false); sm.bits = (unsigned char*)p->screenshoot;
	surface pm; pm.resize(20, 22, 32, false);
	blit(*canvas, caret.x + 6, caret.y + 6, sm.width, sm.height, 0, sm, 0, 0);
	for(auto i = 0; i < 6; i++) {
		pm.bits = (unsigned char*)p->party[i];
		blit(*canvas, caret.x + 514, caret.y + 13, pm.width, pm.height, 0, pm, 0, 0);
		caret.x += 28;
		if(i == 2) {
			caret.x = push.caret.x;
			caret.y += 41;
		}
	}
	sm.bits = 0;
	pm.bits = 0;
	setdialog(140, 15, 345, 18); texta(NORMAL, "268435461", AlignLeft);
	setdialog(140, 40, 279, 18); texta(NORMAL, "268435466", AlignLeft);
	setdialog(604, 11); button(GBTNSTD, 1, 2); fire(row_save, 0, 0, object);
	setdialog(604, 52); button(GBTNSTD, 1, 2); fire(row_delete, 0, 0, object);
}

static void paint_game_list() {
	static int origin;
	const int per_page = 5;
	paint_list(files.data, files.element_size, files.count, origin, per_page, paint_game_row, 102, {}, 0, 0, 0);
}

static void paint_save_game() {
	paint_game_dialog(GUISRSVB);
	setdialog(243, 22, 311, 28); texta(STONEBIG, getnm("SaveGame"), AlignCenterCenter);
	setdialog(23, 78, 740, 498); paint_game_list();
	//setdialog(159, 195, 345, 18); texta(NORMAL, "268435462", AlignCenterCenter);
	//setdialog(159, 220, 279, 18); texta(NORMAL, "268435467", AlignCenterCenter);
	//setdialog(159, 297, 345, 18); texta(NORMAL, "268435463", AlignCenterCenter);
	//setdialog(159, 322, 279, 18); texta(NORMAL, "268435468", AlignCenterCenter);
	//setdialog(159, 399, 345, 18); texta(NORMAL, "268435464", AlignCenterCenter);
	//setdialog(159, 424, 279, 18); texta(NORMAL, "268435469", AlignCenterCenter);
	setdialog(656, 22); button(GBTNSTD, 1, 2, KeyEscape, "Cancel"); fire(buttoncancel);
}

void open_save_game() {
	files.clear();
	auto p = files.add();
	p->create();
	scene(paint_save_game);
}