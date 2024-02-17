#include "bsreq.h"
#include "draw.h"
#include "draw_form.h"
#include "draw_gui.h"
#include "log.h"
#include "screenshoot.h"
#include "script.h"

using namespace draw;

const form* draw::last_form;
static const form* next_last_form;
fnevent form::prepare, form::opening, form::closing;

struct form_parse_key {
	const char*	name;
	unsigned	key;
};
static form_parse_key key_mapping[] = {
	{"Ctrl", Ctrl},
	{"Alt", Alt},
	{"Shift", Shift},
	{"Escape", KeyEscape},
	{"Enter", KeyEnter},
	{"Up", KeyUp},
	{"Down", KeyDown},
	{"Right", KeyRight},
	{"Left", KeyLeft},
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
};

static unsigned parse_hotkey(const char* p) {
	if(!p || p[0] == 0)
		return 0;
	unsigned r = 0;
	auto original_name = p;
	auto need_parse = true;
	while(need_parse) {
		need_parse = false;
		for(auto& e : key_mapping) {
			if(szstart(p, e.name)) {
				p += zlen(e.name);
				if(p[0] == '+')
					p++;
				r |= e.key;
				need_parse = true;
				break;
			}
		}
	}
	auto n = zlen(p);
	if(n == 1) {
		r |= p[0];
		p++;
	}
	if(!r)
		log::error(0, "Can't parse key name `%1`", original_name);
	return r;
}

void form::readhead(const char* url) {
	char temp[260]; szfnamewe(temp, url); szupper(temp);
	auto id = szdup(temp);
	auto p = bsdata<form>::find(id);
	if(!p)
		p = bsdata<form>::add();
	p->id = id;
}

void form::read(const char* url) {
	auto control_start = bsdata<control>::source.getcount();
	bsreq::read(url);
	if(log::geterrors() > 0)
		return;
	char temp[260]; szfnamewe(temp, url); szupper(temp);
	auto id = szdup(temp);
	auto p = bsdata<form>::find(id);
	if(!p)
		p = bsdata<form>::add();
	p->id = id;
	auto control_end = bsdata<control>::source.getcount();
	p->controls = sliceu<control>(control_start, control_end - control_start);
	for(auto& e : p->controls)
		e.key = parse_hotkey(e.hotkey);
}

void form::paint() const {
	static widget* background_widget = bsdata<widget>::find("Background");
	rectpush push;
	auto push_caret = caret;
	auto push_gui = gui;
	for(auto& e : controls) {
		if(e.visual == background_widget) {
			if(e.width || e.height) {
				push_caret.x = (getwidth() - e.width) / 2;
				push_caret.y = (getheight() - e.height) / 4;
			} else {
				push_caret.x += e.x;
				push_caret.y += e.y;
			}
			caret = push_caret;
		} else {
			caret.x = push_caret.x + e.x;
			caret.y = push_caret.y + e.y;
		}
		width = e.width; height = e.height;
		gui.clear();
		gui.id = e.id;
		gui.value = e.value;
		gui.data = e.data;
		gui.key = e.key;
		if(e.resource)
			gui.res = e.resource->get();
		memcpy(gui.frames, e.frames, sizeof(gui.frames));
		auto push_fore = fore;
		if(e.fore.r || e.fore.g || e.fore.b)
			fore = e.fore;
		gui.hilited = ishilite();
		if(prepare)
			prepare();
		e.visual->proc();
		fore = push_fore;
	}
	gui = push_gui;
}

static void form_script(const char* id) {
	auto ps = bsdata<script>::find(str("%1%2", last_form->id, id));
	if(!ps)
		return;
	ps->proc(0);
}

static void form_paint() {
	last_form->paint();
}

long form::open(bool modal) {
	form_script("Opening");
	if(opening)
		opening();
	if(modal)
		screenshoot::open(form_paint, true);
	else
		draw::scene(form_paint);
	if(closing)
		closing();
	form_script("Closing");
	return getresult();
}

long form::open(const char* id, bool modal) {
	auto push_form = last_form;
	last_form = bsdata<form>::find(id);
	long result = 0;
	if(last_form)
		result = last_form->open(modal);
	last_form = push_form;
	return result;
}

static void form_scene() {
	last_form = next_last_form;
	if(last_form)
		last_form->open(false);
}

void form::nextscene() const {
	next_last_form = this;
	setnext(form_scene);
}

void form::nextscene(const char* id) {
	auto p = bsdata<form>::find(id);
	if(p)
		p->nextscene();
}