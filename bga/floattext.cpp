#include "crt.h"
#include "draw.h"
#include "floattext.h"

void floattext::clear() {
	memset(this, 0, sizeof(*this));
}

void floattext::paint() const {
	draw::rectpush push;
	auto push_fore = draw::fore;
	auto push_alpha = draw::alpha;
	draw::width = box.width();
	draw::height = box.height();
	draw::fore = colors::black;
	draw::alpha = 128;
	draw::strokeout(draw::rectf, metrics::border + metrics::padding);
	draw::alpha = push_alpha;
	draw::fore = fore;
	draw::textf(format);
	draw::fore = push_fore;
}

void add_float_text(point position, const char* format, int width, unsigned millisecond) {
	if(!format)
		return;
	auto push_width = draw::width;
	auto push_height = draw::height;
	draw::width = width;
	draw::textfs(format);
	auto p = bsdata<floattext>::addz();
	p->format = format;
	p->box.x1 = position.x - draw::width / 2;
	p->box.y1 = position.y - draw::height - metrics::padding - metrics::border - 4;
	p->box.x2 = p->box.x1 + draw::width;
	p->box.y2 = p->box.y1 + draw::height;
	p->position.x = p->box.x1;
	p->position.y = p->box.y1;
	p->fore = colors::text;
	draw::width = push_width;
	draw::height = push_height;
}