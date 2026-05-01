#include "bsdata.h"
#include "draw.h"
#include "floattext.h"
#include "timer.h"

using namespace draw;

void floattext::clear() {
	memset(this, 0, sizeof(*this));
}

void floattext::paint() const {
	pushrect push;
	auto push_fore = fore;
	auto push_alpha = alpha;
	width = box.width();
	height = box.height();
	::fore = colors::black;
	alpha = 128;
	draw::strokeout(rectf, metrics::border + metrics::padding);
	alpha = push_alpha;
	::fore = this->fore;
	textf(format);
	::fore = push_fore;
}

static floattext* find_exist(const void* data) {
	if(!data)
		return 0;
	for(auto& e : bsdata<floattext>()) {
		if(e.data == data)
			return &e;
	}
	return 0;
}

void add_float_text(point position, const char* format, int width, int millisecond, const void* data) {
	if(!format)
		return;
	auto push_width = draw::width;
	auto push_height = draw::height;
	draw::width = width;
	draw::textfs(format);
	auto p = find_exist(data);
	if(!p)
		p = bsdata<floattext>::addz();
	p->format = format;
	p->box.x1 = position.x - draw::width / 2;
	p->box.y1 = position.y - draw::height - metrics::padding - metrics::border - 4;
	p->box.x2 = p->box.x1 + draw::width;
	p->box.y2 = p->box.y1 + draw::height;
	p->position.x = p->box.x1;
	p->position.y = p->box.y1;
	p->fore = colors::text;
	p->delay = millisecond;
	p->data = data;
	draw::width = push_width;
	draw::height = push_height;
}