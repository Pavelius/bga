#include "draw.h"
#include "creature.h"
#include "resid.h"
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

static void paint_test_table() {
	pushrect push;
	const int dx = 20;
	const int dy = 20;
	paint_game_dialog(GUICHP);
	for(auto y = 0; y < 16; y++) {
		for(auto x = 0; x < 16; x++) {
			caret.x = 100 + x * dx;
			caret.y = 100 + y * dy;
			auto n = (unsigned char)(y * 16 + x);
			glyph(n, 0);
		}
	}
}

static void paint_chapter() {
	paint_game_dialog(GUICHP);
	setdialog(412, 529); button(GBTNBFRM, 1, 2, KeySpace, "Done"); fire(buttonok);
	setdialog(183, 529); button(GBTNBFRM, 1, 2, KeySpace, "Replay");
	setdialog(193, 35, 412, 30); texta(STONEBIG, "Header", AlignCenterCenter);
	setdialog(421, 321, 337, 153); texta(getnm("Description"), AlignCenterCenter);
}

static void textn(resn res, const char* string, unsigned flags) {
	auto ps = gres(res);
	if(!ps)
		return;
	for(auto p = string; *p; p++) {
		auto frame = *((unsigned char*)p) - 2;
		auto& f = ps->get(frame);
		if(f.sx == f.sy && f.sx == 1) {
			caret.x += 5;
			continue;
		}
		image(caret.x, caret.y + 10 - f.oy, ps, frame, ImageNoOffset);
		caret.x += f.sx + 1;
	}
}

static void paint_word_map() {
	paint_dialog(GUIMAP, 2);
	setdialog(61, 157); button(GBTNMED, 1, 2, 0);
	setdialog(61, 187); button(GBTNMED, 1, 2, 0);
	setdialog(61, 217); button(GBTNMED, 1, 2, KeyEscape, "Cancel"); fire(buttoncancel);
	// setdialog(20, 20, 238, 103);
	setdialog(21, 127); button(FLAG1, 1, 0);
	setdialog(51, 127); button(FLAG1, 3, 4);
	setdialog(81, 127); button(FLAG1, 4, 5);
	setdialog(111, 127); button(FLAG1, 0, 2);
	setdialog(141, 127); button(FLAG1, 0, 2);
	setdialog(171, 127); button(FLAG1, 0, 2);
	setdialog(201, 127); button(FLAG1, 0, 2);
	setdialog(231, 127); button(FLAG1, 0, 2);
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
	case Ctrl + 'C': execute(open_scene, 0, 0, paint_test_table); break;
	case Ctrl + 'I': execute(open_scene, 0, 0, util_items_list); break;
	default: break;
	}
}

#else

void input_debug() {
}

#endif // DEBUG