#include "bsreq.h"
#include "draw.h"
#include "draw_control.h"
#include "draw_gui.h"
#include "log.h"
#include "screenshoot.h"

using namespace draw;

form* draw::last_form;
fnevent form::prepare;

bool form::iswindowed() const {
	return controls && controls.begin()[0].height != 0;
}

void form::read(const char* url) {
	auto control_start = bsdata<control>::source.getcount();
	bsreq::read(url);
	if(log::geterrors() > 0)
		return;
	char temp[260]; szfnamewe(temp, url);
	auto p = bsdata<form>::add();
	p->id = szdup(temp);
	auto control_end = bsdata<control>::source.getcount();
	p->controls = sliceu<control>(control_start, control_end - control_start);
}

void form::paint() const {
	rectpush push;
	auto push_caret = caret;
	auto push_gui = gui;
	for(auto& e : controls) {
		if(equal(e.visual->id, "Background")) {
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

static void paintscene() {
	last_form->paint();
}

void form::open(const char* id) {
	auto push_form = last_form;
	last_form = bsdata<form>::find(id);
	if(last_form) {
		if(last_form->iswindowed())
			screenshoot::open(paintscene, true);
		else
			draw::scene(paintscene);
	}
	last_form = push_form;
}

long form::choose(const char* id) {
	auto push_form = last_form;
	last_form = bsdata<form>::find(id);
	if(last_form) {
		if(last_form->iswindowed())
			screenshoot::open(paintscene, true);
		else
			draw::scene(paintscene);
	}
	last_form = push_form;
	return getresult();
}