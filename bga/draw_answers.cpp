#include "answers.h"
#include "draw.h"

using namespace draw;

static bool allow(const answers& an, size_t max_width) {
	for(auto& e : an) {
		if(zlen(e.text) > max_width)
			return false;
	}
	return true;
}

static int getcolumns(const answers& an) {
	auto count = an.getcount();
	if(!count)
		return 1;
	auto result = 1;
	if(count > 3 && (count % 3) == 0 && allow(an, 13))
		result = 3;
	else if((count % 2) == 0)
		result = 2;
	return result;
}

void answers::paintanswers(int columns, const char* cancel_text) const {
	auto column_width = width;
	if(columns > 1)
		column_width = (column_width - (metrics::border * 2 + metrics::padding) * (columns - 1)) / columns + 1;
	auto index = 0;
	auto y1 = (int)caret.y, x1 = (int)caret.x;
	auto y2 = (int)caret.y;
	auto push_width_normal = width;
	width = column_width;
	auto push_x2 = caret.x + width;
	for(auto& e : elements) {
		paintcell(index, e.value, e.text, buttonparam);
		index++;
		if(caret.y > y2)
			y2 = caret.y;
		if(columns > 1) {
			if((index % columns) == 0) {
				y1 = caret.y;
				caret.x = x1;
			} else {
				caret.y = y1;
				caret.x += width + metrics::border * 2 + metrics::padding;
			}
		}
	}
	caret.x = x1; caret.y = y2;
	width = push_width_normal;
	if(cancel_text) {
		auto push_fore = fore;
		fore = fore.mix(colors::h3, 128);
		paintcell(elements.getcount(), 0, cancel_text, buttonparam);
		fore = push_fore;
	}
}

void* answers::choose(const char* title, const char* cancel_text, int cancel_mode) const {
	if(!interactive)
		return random();
	if(cancel_mode == 2 && elements.getcount() == 1)
		return (void*)elements.data[0].value;
	if(!elements) {
		if(!cancel_mode || (cancel_mode && cancel_text == 0))
			return 0;
	}
	auto columns = column_count;
	if(columns == -1)
		columns = getcolumns(*this);
	auto push_title = prompa;
	auto push_caret = caret;
	auto push_width = width;
	auto push_height = height;
	auto push_last = last;
	prompa = title;
	while(ismodal()) {
		paintstart();
		if(beforepaint)
			beforepaint();
		paintanswers(columns, cancel_text);
		if(afterpaint)
			afterpaint();
		width = push_width;
		caret = push_caret;
		paintfinish();
		domodal();
	}
	last = push_last;
	prompa = push_title;
	return (void*)getresult();
}