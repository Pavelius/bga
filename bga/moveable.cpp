#include "crt.h"
#include "draw.h"
#include "moveable.h"

using namespace draw;

static unsigned long timestamp, timestamp_last;
static rect last_screen;

struct draworder {
	moveable*		parent;
	drawable		start;
	draworder*		depend;
	unsigned long	tick_start, tick_stop;
	explicit operator bool() const { return parent != 0; }
	void clear();
	void setduration(int v) { tick_stop = tick_start + v; }
	void update();
	void wait();
};

static void start_timer() {
	timestamp_last = getcputime();
}

static void update_timestamp() {
	auto c = getcputime();
	if(!timestamp_last || c < timestamp_last)
		timestamp_last = c;
	timestamp += c - timestamp_last;
	timestamp_last = c;
}

static int calculate(int v1, int v2, int n, int m) {
	return v1 + (v2 - v1) * n / m;
}

static int compare(const void* v1, const void* v2) {
	auto p1 = *((moveable**)v1);
	auto p2 = *((moveable**)v2);
	auto a1 = p1->priority / 10;
	auto a2 = p2->priority / 10;
	if(a1 != a2)
		return a1 - a2;
	if(p1->y != p2->y)
		return p1->y - p2->y;
	return p1->x - p2->x;
}

static void sortobjects(moveable** pb, size_t count) {
	qsort(pb, count, sizeof(pb[0]), compare);
}

static void paint_objects() {
	static moveable* source[512];
	auto push_caret = caret;
	auto push_clip = clipping;
	last_screen = {caret.x, caret.y, caret.x + width, caret.y + height};
	setclip(last_screen);
	auto count = 0;// getobjects(source, source + sizeof(source) / sizeof(source[0]));
	sortobjects(source, count);
	for(size_t i = 0; i < count; i++) {
		draw::caret = source[i]->getscreen();
		source[i]->paint();
	}
	clipping = push_clip;
	caret = push_caret;
}

point drawable::getscreen() const {
	return *this - camera;
}