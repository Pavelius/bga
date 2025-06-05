#include "area.h"
#include "animation.h"
#include "bsreq.h"
#include "calendar.h"
#include "class.h"
#include "creature.h"
#include "colorgrad.h"
#include "console.h"
#include "container.h"
#include "door.h"
#include "draw.h"
#include "draw_focus.h"
#include "drawable.h"
#include "floattext.h"
#include "game.h"
#include "iteminside.h"
#include "itemlist.h"
#include "list.h"
#include "map.h"
#include "math.h"
#include "race.h"
#include "region.h"
#include "resinfo.h"
#include "script.h"
#include "scrollable.h"
#include "store.h"
#include "stringvar.h"
#include "timer.h"
#include "view.h"
#include "widget.h"
#include "worldmap.h"

using namespace draw;

const char* getkg(int weight);
extern array console_data;

const int tile_size = 64;

static item drag_item;
item *drag_item_source, *drag_item_dest;
static int current_info_tab;
static bool need_update, button_run;
static scrolltext area_description, area_console;
static resinfo default_cursor;
static void *current_topic;
static worldmapi::area* current_world_area_hilite;
static partyitemlist party_items;
static partyitemlist store_items;

static void form_opening() {
	invalidate_description();
	default_cursor = cursor;
}

static void form_closing() {
	cursor = default_cursor;
}

static void set_value_and_update() {
	cbsetint();
	invalidate_description();
}

static void paint_cursor() {
	auto pi = gres(cursor.id);
	if(!pi)
		return;
	auto cicle = cursor.cicle;
	if(cursor.id == CURSORS) {
		auto pressed = hot.pressed;
		if(pressed)
			cicle += 1;
		image(hot.mouse.x, hot.mouse.y, pi, cicle, 0);
	} else if(cursor.id == CURSARW) {
		auto ti = pi->ganim(cursor.cicle, current_tick / 32);
		image(hot.mouse.x, hot.mouse.y, pi, ti, 0);
	} else
		image(hot.mouse.x, hot.mouse.y, pi, cicle, 0);
}

static void paint_background() {
	update_tick();
}

static void paint_logs(const char* format, int& origin, int& format_origin, int& maximum) {
	if(!format)
		return;
	pushrect push;
	if(format_origin == -1) {
		textfs(format);
		maximum = height;
		height = push.height;
		width = push.width;
		caret = push.caret;
		if(maximum > height)
			caret.y -= maximum - height;
	}
	textf(format, origin, format_origin);
}

void widget::initialize() {
	pbackground = paint_background;
	ptips = paint_cursor;
	cursor.set(CURSORS, 0);
	draw::syscursor(false);
}

void widget::open() const {
	scene(proc);
}

static void paint_draggable() {
	if(drag_item_source) {
		if(hot.key == MouseRight
			|| hot.key == KeyEscape
			|| (hot.key == MouseLeft && hot.pressed))
			execute(buttoncancel);
	}
	drag_item_dest = 0;
}

static creature* getowner(const item* p) {
	auto i = bsdata<creature>::source.indexof(p);
	return (i == -1) ? 0 : (creature*)bsdata<creature>::source.ptr(i);
}

static void begin_drag_item() {
	auto push_cursor = cursor;
	auto push_drag = drag_item_source;
	drag_item_source = (item*)hot.object;
	drag_item = *drag_item_source;
	drag_item_source->clear();
	update_creature();
	cursor.id = ITEMS;
	cursor.cicle = drag_item.geti().avatar * 2 + 1;
	scene(paint_draggable);
	if(!drag_item_dest)
		*drag_item_source = drag_item;
	else if(player->have(drag_item_dest)) {
		if(*drag_item_dest)
			player->additem(*drag_item_dest);
		*drag_item_dest = drag_item;
	} else {
		auto pn = getowner(drag_item_dest);
		if(pn)
			pn->additem(drag_item);
	}
	drag_item_source = push_drag;
	cursor = push_cursor;
	update_creature();
}

static void allow_drop_target(item* pi, wear_s slot) {
	//if(drag_item_source) {
	//	if(gui.hilited) {
	//		drag_item_dest = pi;
	//		image(gui.res, 25, 0);
	//	}
	//}
}

//static void paint_drop_target(item* pi, wear_s slot) {
//	if(drag_item_source && drag_item.canequip(slot)) {
//		if(player->isusable(drag_item)) {
//			if(gui.hilited) {
//				image(gui.res, 25, 0);
//				drag_item_dest = pi;
//			} else
//				image(gui.res, 16, 0);
//		}
//	}
//}

//static void creature_ability_bonus() {
//	if(gui.data.iskind<abilityi>())
//		gui.value = player->getbonus((ability_s)gui.data.value);
//	char temp[32]; stringbuilder sb(temp);
//	auto push_fore = fore;
//	if(gui.value > 0) {
//		sb.add("%+1i", gui.value);
//		fore = colors::green;
//	} else if(gui.value < 0) {
//		fore = colors::red;
//		sb.add("%+1i", gui.value);
//	} else
//		sb.add("%+1i", gui.value);
//	gui.text = temp;
//	label();
//	fore = push_fore;
//}

//static void text_description() {
//	auto push_font = font;
//	if(gui.res)
//		font = gui.res;
//	area_description.paint(description_text);
//	if(gui.hilited)
//		area_description.input();
//	font = push_font;
//}

//static void list_elements(void** current_focus) {
//	if(!gui.data.iskind<listi>())
//		return;
//	auto ps = bsdata<listi>::elements + gui.data.value;
//	if(!ps->elements.size())
//		return;
//	if(!(*current_focus))
//		*current_focus = ps->elements.begin()->getpointer();
//	auto max_height = caret.y + draw::height;
//	draw::height = texth() + 2;
//	for(auto v : ps->elements) {
//		if(caret.y >= max_height)
//			break;
//		auto p = v.getpointer();
//		gui.text = v.getname();
//		gui.checked = (p == *current_focus);
//		gui.hilited = ishilite();
//		label_left();
//		button_run_input();
//		if(button_run)
//			execute(set_ptr_and_invalidate, (long)p, 0, current_focus);
//		caret.y += draw::height;
//	}
//}
//
//static void list_elements(void** current_focus, const array& source) {
//	if(!last_scroll)
//		return;
//	last_scroll->perscreen = height;
//	last_scroll->perline = texth() + 2;
//	last_scroll->maximum = (source.getcount() + 1) * last_scroll->perline + 1;
//	last_scroll->correct();
//	if(gui.hilited)
//		last_scroll->input();
//	if(!(*current_focus))
//		*current_focus = source.begin();
//	auto push_clip = clipping;
//	setclipall();
//	auto size = source.element_size;
//	auto p = source.begin() + size * (last_scroll->origin / last_scroll->perline);
//	auto pe = source.end();
//	auto max_height = caret.y + draw::height;
//	draw::height = last_scroll->perline;
//	while(p < pe) {
//		if(caret.y >= max_height)
//			break;
//		gui.text = ((nameable*)p)->getname();
//		gui.checked = (p == *current_focus);
//		gui.hilited = ishilite();
//		label_left();
//		button_run_input();
//		if(button_run)
//			execute(set_ptr_and_invalidate, (long)p, 0, current_focus);
//		caret.y += last_scroll->perline;
//		p += size;
//	}
//	clipping = push_clip;
//}
//
//static void list_collection(void** current_focus, const collectiona& source, scrollable& scroll) {
//	last_scroll = &scroll;
//	last_scroll->perline = 1;
//	last_scroll->perscreen = gui.value;
//	last_scroll->maximum = source.getcount();
//	if(gui.hilited)
//		last_scroll->input();
//	if(!(*current_focus))
//		*current_focus = source[0];
//}

static void store_list() {
	last_scroll = 0;
	//static scrolllist scroll;
	//list_items.clear();
	//load_items(list_items, last_store);
	//list_collection((void**)&store_item, list_items, scroll);
}

static void player_items() {
	static creature* current_player;
	if(current_player != player) {
		current_player = player;
		party_items.invalidate();
	}
	party_items.paint();
}

static itemlist* get_item_list() {
	if(!last_scroll)
		return 0;
	return static_cast<itemlist*>(last_scroll);
}

//static itemlist::element* get_element() {
//	if(!last_scroll)
//		return 0;
//	auto n = gui.value + last_scroll->origin;
//	if(n >= last_scroll->maximum)
//		return 0;
//	itemlist* pn = static_cast<itemlist*>(last_scroll);
//	return pn->data + n;
//}

//static void store_item_avatar() {
//	auto pi = get_element();
//	if(!pi)
//		return;
//	auto index = gui.value % 8;
//	if(index >= 4)
//		index += 4;
//	image(gui.res, index, 0);
//	if(gui.hilited) {
//		switch(hot.key) {
//		case MouseLeft:
//			if(hot.pressed)
//				execute(cbsetint, pi->count + 1, 0, &pi->count);
//			break;
//		case MouseLeftDBL:
//			break;
//		case MouseRight:
//			if(pi->count > 0 && hot.pressed)
//				execute(cbsetint, pi->count - 1, 0, &pi->count);
//			break;
//		}
//	}
//	//if(*pi->data)
//	//	paint_item(pi->data);
//	if(pi->count > 0)
//		image(gui.res, 25, 0);
//}

//static void store_item_info() {
//	auto pi = get_element();
//	if(!pi)
//		return;
//	format_label("%1 - %2i%3", pi->data->getname(), pi->data->getcost(), getnm("GP"));
//	input_item_info(pi->data);
//}