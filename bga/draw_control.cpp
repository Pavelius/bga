#include "bsreq.h"
#include "draw.h"
#include "draw_control.h"
#include "draw_gui.h"
#include "log.h"

using namespace draw;

form* draw::last_form;
fnevent form::prepare;

void form::read(const char* url) {
	auto control_start = bsdata<control>::source.getcount();
	bsreq::read(url);
	if(log::geterrors() > 0)
		return;
	char temp[260]; szfnamewe(temp, url);
	auto p = bsdata<form>::add();
	p->id = szdup(temp);
	auto control_end = bsdata<control>::source.getcount();
	p->controls = sliceu<control>(control_start, control_end);
}

void form::paint() const {
	rectpush push;
	auto push_gui = gui;
	for(auto& e : controls) {
		caret.x = push.caret.x + e.x;
		caret.y = push.caret.y + e.y;
		width = e.width; height = e.height;
		gui.clear();
		gui.id = e.id;
		gui.value = e.value;
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

void form::paintscene() {
	last_form->paint();
}