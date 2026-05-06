#include "math.h"
#include "sprite_util.h"
#include "slice.h"
#include "win.h"

using namespace util;

#define NONANTIALIASED_QUALITY  3
#define ANTIALIASED_QUALITY     4

static HDC hcnv, hfnt;
static unsigned short ascii_decoder[256];
static int height_diff;

static void wcpy(wchar_t* d, const char* s) {
	while(*s)
		*d++ = *s++;
	*d++ = 0;
}

static void font_create(const char* name, int size) {
	wchar_t name1[260]; wcpy(name1, name);
	void* hf = CreateFontW(size, 0, 0, 0, 400, 0, 0,
		0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY, FF_DONTCARE,
		name1);
	hcnv = CreateCompatibleDC(0);
	hfnt = SelectObject(hcnv, hf);
}

static void font_delete() {
	DeleteObject(SelectObject(hcnv, hfnt));
	DeleteDC(hcnv);
}

// tool used to create font with SubPixel rendering from system font's
static int font_glyphi(int glyph, int& width, int& height, int& dx, int& dy, int& ox, int& oy, unsigned char* buffer, int maxsize, int xscale, int yscale) {
	MAT2 scale3h = {0};
	scale3h.eM11.value = xscale; // rgb color
	scale3h.eM22.value = yscale;
	GLYPHMETRICS gm = {0};
	int size = GetGlyphOutlineW(hcnv,
		glyph,
		GGO_BITMAP,
		&gm,
		maxsize,
		buffer,
		&scale3h);
	width = gm.gmBlackBoxX;
	height = gm.gmBlackBoxY;
	dx = gm.gmCellIncX;
	dy = gm.gmCellIncY;
	ox = gm.gmptGlyphOrigin.x;
	oy = gm.gmptGlyphOrigin.y;
	return size;
}

static void font_info(int& width, int& height, int& dy1, int& dy2) {
	TEXTMETRICA tm;
	GetTextMetricsA(hcnv, &tm);
	width = tm.tmAveCharWidth;
	height = tm.tmHeight;
	dy1 = tm.tmAscent;
	dy2 = tm.tmDescent;
}

static unsigned char buf1[128 * 128];
static unsigned char buf2[128 * 128];
static short int widths[4096];

static void initialize_ascii_decoder() {
	memset(ascii_decoder, 0, sizeof(ascii_decoder));
	for(auto i = 0; i < 128; i++)
		ascii_decoder[i] = i;
	for(auto i = 0xA0; i < 256; i++)
		ascii_decoder[i] = i;
	ascii_decoder[149] = 0x2022;
	ascii_decoder[165] = 0x490;
	ascii_decoder[168] = 0x401;
	ascii_decoder[170] = 0x404;
	ascii_decoder[175] = 0x407;
	ascii_decoder[178] = 0x406;
	ascii_decoder[179] = 0x456;
	ascii_decoder[180] = 0x491;
	ascii_decoder[184] = 0x451;
	ascii_decoder[186] = 0x454;
	ascii_decoder[191] = 0x457;
	for(auto i = 0xC0; i < 256; i++)
		ascii_decoder[i] = 0x410 + (i - 0xC0);
}

static void put_mono(unsigned char* d, unsigned dn, unsigned char* s, unsigned sn, int w, int h) {
	for(auto y = 0; y < h; y++) {
		for(auto x = 0; x < w; x++) {
			unsigned char n = 0;
			if(s[x / 8] & (0x80 >> (x % 8)))
				n = 1;
			d[x] = n;
		}
		s += sn;
		d += dn;
	}
}

static void put_gray(unsigned char* d, int dn, unsigned char* s, int sn, int w, int h, unsigned char sym) {
	while(h-- > 0) {
		for(auto x = 0; x < w; x++) {
			if(s[x])
				d[x] = sym;
		}
		d += dn;
		s += sn;
	}
}

static void put_gray(unsigned char* d, int dn, unsigned char* s, int sn, int w, int h, unsigned char sym, unsigned char sym_dest) {
	while(h-- > 0) {
		for(auto x = 0; x < w; x++) {
			if(s[x] == sym)
				d[x] = sym_dest;
			else
				d[x] = 0;
		}
		d += dn;
		s += sn;
	}
}

static void glyph_mono(int g, sprite& ei, sprite::encodes encode, int index) {
	const int glyph_start = 32;
	const int glyph_count = 256 - glyph_start;
	int dx, dy, ox, oy, w, h;
	memset(buf1, 0, sizeof(buf1));
	memset(buf2, 0, sizeof(buf2));
	int size = font_glyphi(g, w, h, dx, dy, ox, oy, buf1, sizeof(buf1), 1, 1);
	if(size <= 0)
		return;
	auto w1 = w + 4, h1 = h + 4;
	auto sn = w1;
	ox++; oy++;
	put_mono(buf2, sn, buf1, ((w + 31) / 32) * 4, w, h);
	memset(buf1, 0, sizeof(buf1));
	put_gray(buf1 + 0 + 0 * sn, sn, buf2, sn, w, h, 3);
	put_gray(buf1 + 1 + 0 * sn, sn, buf2, sn, w, h, 3);
	put_gray(buf1 + 2 + 0 * sn, sn, buf2, sn, w, h, 3);
	put_gray(buf1 + 0 + 1 * sn, sn, buf2, sn, w, h, 3);
	put_gray(buf1 + 0 + 2 * sn, sn, buf2, sn, w, h, 3);
	put_gray(buf1 + 2 + 1 * sn, sn, buf2, sn, w, h, 2);
	put_gray(buf1 + 2 + 2 * sn, sn, buf2, sn, w, h, 2);
	put_gray(buf1 + 1 + 2 * sn, sn, buf2, sn, w, h, 2);
	put_gray(buf1 + 1 + 1 * sn, sn, buf2, sn, w, h, 1);
	// Copy back image
	memset(buf2, 0, sizeof(buf2)); put_gray(buf2, sn, buf1, sn, w1, h1, 1, 1);
	sprite_store(&ei, buf2, sn, w1, h1, -ox, oy - height_diff, encode, 0, 0, index, 0);
	memset(buf2, 0, sizeof(buf2)); put_gray(buf2, sn, buf1, sn, w1, h1, 2, 1);
	sprite_store(&ei, buf2, sn, w1, h1, -ox, oy - height_diff, encode, 0, 0, index + glyph_count * 1, 0);
	memset(buf2, 0, sizeof(buf2)); put_gray(buf2, sn, buf1, sn, w1, h1, 3, 1);
	sprite_store(&ei, buf2, sn, w1, h1, -ox, oy - height_diff, encode, 0, 0, index + glyph_count * 2, 0);
	widths[index] = dx;
}

void font_write_ascii(const char* url, const char* name, int size, sprite::encodes encode) {
	font_create(name, size);
	auto p = (sprite*)new char[256 * 256 * 8];
	if(!p)
		return;
	initialize_ascii_decoder();
	const int glyph_start = 32;
	const int glyph_count = 256 - glyph_start;
	sprite_create(p, glyph_count * 3);
	int height, width, ascend, descend;
	font_info(width, height, ascend, descend);
	p->height = height;
	p->ascend = ascend;
	p->descend = descend;
	height_diff = p->height - p->descend;
	for(auto i = glyph_start; i < 256; i++) {
		auto g = i - glyph_start;
		auto u = ascii_decoder[i];
		glyph_mono(u, *p, encode, g);
	}
	sprite_add(p, widths, sizeof(widths[0]) * glyph_count);
	p->width = widths['l' - glyph_start];
	sprite_write(url, p);
	font_delete();
	delete[] p;
}