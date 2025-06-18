#include "audio.h"
#include "creature.h"
#include "draw.h"
#include "view.h"

using namespace draw;

static void paint_character_generation() {
	paint_game_dialog(GUICGB);
	setdialog(174, 22, 452, 29); texta(STONEBIG, getnm("CharacterGeneration"), AlignCenterCenter);
	setdialog(173, 65, 453, 20); texta(NORMAL, "Main Menu", AlignCenterCenter);
	setdialog(274, 113); button(GBTNLRG, 1, 2);
	setdialog(274, 148); button(GBTNLRG, 5, 6);
	setdialog(274, 183); button(GBTNLRG, 9, 10);
	setdialog(274, 218); button(GBTNLRG, 13, 14);
	setdialog(274, 253); button(GBTNLRG, 1, 2);
	setdialog(274, 288); button(GBTNLRG, 5, 6);
	setdialog(274, 323); button(GBTNLRG, 9, 10);
	setdialog(274, 358); button(GBTNLRG, 13, 14);
	// setdialog(23, 151); button(GBTNSTD, 0, 1);
	setdialog(35, 550); button(GBTNSTD, 1, 2, 0, "Biography", 3, false);
	setdialog(204, 550); button(GBTNSTD, 5, 6, 0, "Import", 7, false);
	setdialog(342, 550); button(GBTNSTD, 9, 10, KeyEscape, "Back");
	setdialog(478, 550); button(GBTNSTD, 1, 2, KeyEnter, "Next");
	setdialog(647, 550); button(GBTNSTD, 5, 6, 0, "Cancel", 7, false);
	//setdialog(568, 153, 188, 325); texta(NORMAL, "", AlignCenterCenter);
	//Scroll GBTNSCRL 765 151 12 330 frames(1 0 3 2 4 5)
	audio_update_channels();
}

void open_character_generation() {
	auto push_player = player;
	creature copy = {}; player = &copy;
	scene(paint_character_generation);
	player = push_player;
}