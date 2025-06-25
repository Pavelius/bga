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

static void paint_glyph(unsigned char n) {
	if(n <= glyph_start)
		return;
	pushfore push_fore;
	auto g = n - glyph_start;
	auto& f = font->get(g);
	fore = push_fore.fore;
	image(font, g, 0);
	// fore = push_fore.fore.mix(fore_stroke, 128);
	// image(font, g + glyph_count * 1, 0);
	// fore = fore_stroke;
	// image(font, g + glyph_count * 2, 0);
	// fore = push_fore.fore.mix(fore_stroke, 64);
	// image(font, g + glyph_count * 3, 0);
}

static int get_glyph_width(unsigned char n) {
	if(n <= glyph_start)
		n = 'l';
	auto widths = (short*)font->ptr(font->size - glyph_count * 2);
	return widths[n - glyph_start];
}

static void test_text(const char* p) {
	pushfont push_font(gres(TEST));
	while(*p) {
		auto symbol = *((unsigned char*)p);
		paint_glyph(symbol);
		caret.x += get_glyph_width(symbol);
		p++;
	}
}

static void paint_test_table() {
	pushrect push;
	pushfont push_font(gres(TEST));
	const int dx = 20;
	const int dy = 20;
	paint_game_dialog(GUICHP);
	for(auto y = 0; y < 16; y++) {
		for(auto x = 0; x < 16; x++) {
			caret.x = 100 + x * dx;
			caret.y = 100 + y * dy;
			auto n = (unsigned char)(y * 16 + x);
			paint_glyph(n);
		}
	}
}

static void paint_test_text() {
	paint_game_dialog(GUICHP);
	caret.x = 100;
	caret.y = 100;
	test_text("Test string to output");
	caret.x = 100;
	caret.y = 120;
	test_text("А вот это строка на русском языке. Ее много и так.");
	caret.x = 100;
	caret.y = 140;
	test_text("А ось ця строка на українській мові. Її і так багато і не дуже.");
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
	case Ctrl + 'C': execute(open_scene, 0, 0, paint_test_text); break;
	case Ctrl + 'I': execute(open_scene, 0, 0, util_items_list); break;
	default: break;
	}
}

#else

void input_debug() {
}

#endif // DEBUG