#include "draw.h"
#include "resid.h"
#include "view.h"

using namespace draw;

#ifdef _DEBUG

void util_items_list();

static void paint_chapter() {
	paint_game_dialog(GUICHP);
	setdialog(412, 529); button(GBTNBFRM, 1, 2, KeySpace, "Done"); fire(buttonok);
	setdialog(183, 529); button(GBTNBFRM, 1, 2, KeySpace, "Replay");
	setdialog(193, 35, 412, 30); texta(STONEBIG, "Header", AlignCenterCenter);
	setdialog(421, 321, 337, 153); texta(getnm("Description"), AlignCenterCenter);
}

static void paint_word_map() {
	paint_dialog(GUIMAP, 2);
	setdialog(61, 157); button(GBTNMED, 1, 2, 0);
	setdialog(61, 187); button(GBTNMED, 1, 2, 0);
	setdialog(61, 217); button(GBTNMED, 1, 2, KeyEscape, "Cancel"); fire(buttoncancel);
	// Unknown None 20 20 238 103 
	setdialog(21, 127); button(FLAG1, 1, 0);
	setdialog(51, 127); button(FLAG1, 3, 4);
	setdialog(81, 127); button(FLAG1, 4, 5);
	setdialog(111, 127); button(FLAG1, 0, 2);
	setdialog(141, 127); button(FLAG1, 0, 2);
	setdialog(171, 127); button(FLAG1, 0, 2);
	setdialog(201, 127); button(FLAG1, 0, 2);
	setdialog(231, 127); button(FLAG1, 0, 2);
}

void input_debug() {
	switch(hot.key) {
	case Ctrl + 'D': execute(open_dialog, 1, 0, paint_word_map); break;
	case Ctrl + 'I': execute(open_scene, 0, 0, util_items_list); break;
	default: break;
	}
}

#else

void input_debug() {
}

#endif // DEBUG