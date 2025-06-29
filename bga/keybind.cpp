#include "bsdata.h"
#include "bsreq.h"
#include "draw.h"
#include "keybind.h"
#include "script.h"

using namespace draw;

BSDATAD(keybind)

BSMETA(keybind) = {
	BSREQ(key_string),
	BSREQ(command),
	{}};

namespace {
struct keyname {
	const char* id;
	unsigned key;
};
}

static keyname keys[] = {
	{"Backspace", KeyBackspace},
	{"Esc", KeyEscape},
	{"Enter", KeyEnter},
	{"F1", F1},
	{"F2", F2},
	{"F3", F3},
	{"F4", F4},
	{"F5", F5},
	{"F6", F6},
	{"F7", F7},
	{"F8", F8},
	{"F9", F9},
	{"F10", F10},
	{"F11", F11},
	{"F12", F12},
	{"Del", KeyDelete},
	{"Space", KeySpace},
	{"Tab", KeyTab},
};

static unsigned get_key(const char* p) {
	for(auto& e : keys) {
		if(equal(p, e.id))
			return e.key;
	}
	return p[0];
}

static bool prefix(const char*& p, const char* id) {
	if(!szstart(p, id))
		return false;
	p += zlen(id);
	if(*p == '+')
		p++;
	return true;
}

static unsigned parse_key(const char* p) {
	unsigned r = 0;
	if(prefix(p, "Ctrl"))
		r |= Ctrl;
	if(prefix(p, "Shift"))
		r |= Shift;
	if(prefix(p, "Alt"))
		r |= Alt;
	r |= get_key(p);
	return r;
}

static int compare(const void* v1, const void* v2) {
	auto p1 = (keybind*)v1;
	auto p2 = (keybind*)v2;
	return p1->key - p2->key;
}

void initialize_keybind() {
	for(auto& e : bsdata<keybind>())
		e.key = parse_key(e.key_string);
	bsdata<keybind>::source.sort(compare);
}

static keybind* find_key(unsigned key) {
	//keybind e = {};	e.key = key;
	//return (keybind*)bsearch(&e, bsdata<keybind>::source.data, bsdata<keybind>::source.count, bsdata<keybind>::source.element_size, compare);
	for(auto& e : bsdata<keybind>()) {
		if(e.key == key)
			return &e;
	}
	return 0;
}

void apply_keybinding() {
	auto p = find_key(hot.key);
	if(p)
		script_run(p->command);
}