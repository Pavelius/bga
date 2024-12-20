#pragma once

struct color {
	unsigned char			b;
	unsigned char			g;
	unsigned char			r;
	unsigned char			a;
	//
	color() = default;
	constexpr color(unsigned char r, unsigned char g, unsigned char b) : b(b), g(g), r(r), a(0) {}
	constexpr color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : b(b), g(g), r(r), a(a) {}
	inline bool	operator==(const color& e) const { return b == e.b && g == e.g && r == e.r && a == e.a; }
	inline bool	operator!=(const color& e) const { return b != e.b || g != e.g || r != e.r || a != e.a; }
	//
	inline void				clear() { *((int*)this) = 0; }
	static void				convert(void* output, int width, int height, int output_bpp, const void* output_pallette, const void* input, int input_bpp, const void* input_pallette = 0, int input_scanline = 0);
	color					darken() const;
	int						find(const void* pallette, int count) const;
	static void				flipv(unsigned char* bits, unsigned scanline, int height);
	char*					getname(char* result, const char* result_maximum) const;
	color					gray() const;
	color					lighten() const;
	bool					isdark() const;
	color					mix(const color c1, unsigned char s = 128) const;
	color					negative() const;
	void					read(const void* scanline, int x, int bpp, const void* pallette = 0);
	static void				rgb2bgr(color* source, int count);
	static int				scanline(int width, int bpp);
	void					write(void* scanline, int x, int bpp, const void* pallette = 0, int color_count = 0) const;
};
color operator*(color c1, color c2);
namespace colors {
extern color				black;
extern color				blue;
extern color				gray;
extern color				green;
extern color				red;
extern color				yellow;
extern color				white;
}
