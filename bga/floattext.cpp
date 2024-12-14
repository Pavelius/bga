#include "draw.h"
#include "floattext.h"
#include "timer.h"

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

static floattext* find_exist(const void* data) {
	if(!data)
		return 0;
	for(auto& e : bsdata<floattext>()) {
		if(e.data == data)
			return &e;
	}
	return 0;
}

void add_float_text(point position, const char* format, int width, unsigned millisecond, const void* data) {
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
	p->stop_visible = current_game_tick + millisecond;
	p->data = data;
	draw::width = push_width;
	draw::height = push_height;
}