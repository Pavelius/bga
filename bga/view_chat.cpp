#include "ambient.h"
#include "answers.h"
#include "audio.h"
#include "console.h"
#include "creature.h"
#include "draw.h"
#include "formation.h"
#include "game.h"
#include "pushvalue.h"
#include "screenshoot.h"
#include "timer.h"
#include "list.h"
#include "math.h"
#include "variant.h"
#include "vector.h"
#include "view.h"
#include "view_list.h"

using namespace draw;

static void portrait_small() {
	if(!player)
		return;
	image(pma_port[0], player->portrait, 0);
}

static void view_chat_area() {
	paint_game_dialog(0, 316, "GUWBTP", 2);
	setdialog(31, 25); portrait_small();
	setdialog(706, 26, 76, 30); texta(str("%1i gp", player->coins), AlignRightBottom);
	setdialog(120, 20, 536, 204); paint_description(21, -11, 20);
	if(!an) {
		setdialog(120 + (536 - 205) / 2, 254);
		button(pma_butlrg2, 1, 2, KeyEnter, "Next");
		fire(buttoncancel);
	}
}

static void view_chat_dialog() {
	update_tick();
	audio_update_channels();
	update_area_music();
	update_ambients(camera);
	setcaret(0, 0, 800, 316);
	paint_area_no_command();
	view_chat_area();
}

void* choose_answers() {
	return scene(view_chat_dialog);
}