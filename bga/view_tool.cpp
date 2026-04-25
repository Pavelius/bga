#include "audio.h"
#include "draw.h"
#include "creature.h"
#include "view.h"

using namespace draw;

#ifdef _DEBUG

const int glyph_start = 32;
const int glyph_count = 256 - glyph_start;

void util_items_list();

static void count_colors_rle832(unsigned* colors, unsigned char* s, int h) {
	while(true) {
		unsigned char c = *s++;
		if(c == 0) {
			if(--h == 0)
				break;
		} else if(c <= 0x9F) {
			unsigned char cb;
			// count
			if(c <= 0x7F)
				cb = c;
			else if(c == 0x80)
				cb = *s++;
			else
				cb = c - 0x80;
			// visible part
			do {
				colors[*s++]++;
			} while(--cb);
		} else {
			if(c == 0xA0)
				s++;
		}
	}
}

static void count_colors(sprite* p) {
	unsigned colors[256] = {};
	for(auto i = 33; i < 125; i++) {
		auto& f = p->get(i);
		if(!f.offset)
			continue;
		count_colors_rle832(colors, (unsigned char*)p->ptr(f.offset), f.sy);
	}
}

static void view_chapter() {
	play_music("MX2102A");
	auto pb1 = gres("GBTNBFRM");
	paint_game_dialog("GUICHP");
	setdialog(412, 529); button(pb1, 1, 2, KeySpace, "Done"); fire(buttonok);
	setdialog(183, 529); button(pb1, 1, 2, KeySpace, "Replay");
	setdialog(193, 35, 412, 30); texta(metrics::h1, "Header", AlignCenterCenter);
	setdialog(421, 321, 337, 153); texta(getnm("Description"), AlignCenterCenter);
}

static void view_area_info() {
	auto pb1 = gres("GBTNMED");
	auto pb2 = gres("FLAG1");
	paint_dialog("GUIMAP", 2);
	setdialog(61, 157); button(pb1, 1, 2, 0);
	setdialog(61, 187); button(pb1, 1, 2, 0);
	setdialog(61, 217); button(pb1, 1, 2, KeyEscape, "Cancel"); fire(buttoncancel);
	// setdialog(20, 20, 238, 103);
	setdialog(21, 127); button(pb2, 1, 0);
	setdialog(51, 127); button(pb2, 3, 4);
	setdialog(81, 127); button(pb2, 4, 5);
	setdialog(111, 127); button(pb2, 0, 2);
	setdialog(141, 127); button(pb2, 0, 2);
	setdialog(171, 127); button(pb2, 0, 2);
	setdialog(201, 127); button(pb2, 0, 2);
	setdialog(231, 127); button(pb2, 0, 2);
}

static void test_battle_stance() {
	player->readybattle(!player->is(ReadyToBattle));
}

static void test_animation_hit() {
	if(player == party[0])
		player->animateattack(party[1]);
	else
		player->animateattack(party[0]);
}

static void test_animation_hit_drop() {
	player->animatedamage();
}

void input_debug() {
	switch(hot.key) {
	case 'A': execute(test_animation_hit); break;
	case 'Q': execute(test_animation_hit_drop); break;
	case 'W': execute(test_battle_stance); break;
	case Ctrl + 'D': execute(open_store); break;
	case Ctrl + 'C': execute(open_container, 0, 0); break;
	case Ctrl + 'I': execute(open_scene, 0, 0, util_items_list); break;
	case Ctrl + 'F': execute(open_scene, 0, 0, view_chapter); break;
	default: break;
	}
}

#else

void input_debug() {
}

#endif // DEBUG