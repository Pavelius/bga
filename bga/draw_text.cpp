#include "draw.h"

const int glyph_start = 32;
const int glyph_count = 256 - glyph_start;

void draw::glyph(int n, unsigned flags) {
	if(n <= glyph_start)
		return;
	image(font, n - glyph_start, 0);
}

int draw::textw(int n) {
	if(n <= glyph_start)
		n = 'l';
	auto widths = (short*)font->ptr(font->size - glyph_count * 2);
	return widths[n - glyph_start];
}

int draw::texth() {
	if(!font)
		return 0;
	return font->height;
}