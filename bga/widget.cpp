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
#include "draw_form.h"
#include "draw_gui.h"
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
static form* next_last_form;
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
	form::opening = form_opening;
	form::closing = form_closing;
	cursor.set(CURSORS, 0);
	draw::syscursor(false);
}

void widget::open() const {
	scene(proc);
}

static void background() {
	image(gui.res, gui.value, 0);
}

static const char* format_string(const char* format) {
	static char temp[512]; stringbuilder sb(temp);
	sb.add(format);
	return temp;
}

static const char* getname() {
	const char* pn = 0;
	if(gui.text)
		pn = gui.text;
	if(!pn && gui.id)
		pn = format_string(getnm(gui.id));
	if(!pn && gui.data)
		pn = gui.data.getname();
	if(!pn)
		pn = "";
	return pn;
}

static void execute_script() {
	auto p = (script*)hot.object;
	p->proc(hot.param);
}

static void open_form() {
	auto push_form = last_form;
	last_form = (form*)hot.object;
	switch(hot.param) {
	case 1: last_form->nextscene(); break;
	case 2: last_form->open(true); break;
	default: last_form->open(false); break;
	}
	last_form = push_form;
}

static void open_widget() {
	auto p = (widget*)hot.object;
	p->open();
}

static void execute_action(variant v, int value) {
	if(v.iskind<form>())
		execute(open_form, value, 0, bsdata<form>::elements + v.value);
	else if(v.iskind<widget>())
		execute(open_widget, value, 0, bsdata<widget>::elements + v.value);
	else if(v.iskind<script>())
		execute(execute_script, value, 0, bsdata<script>::elements + v.value);
}

static void button_run_input() {
	button_run = false;
	if(gui.disabled)
		return;
	if(gui.key && hot.key == gui.key)
		button_run = true;
	if(gui.hilited && hot.key == MouseLeft && !hot.pressed)
		button_run = true;
}

static void button_input() {
	button_run_input();
	if(button_run)
		execute_action(gui.data, gui.value);
}

static void button(const sprite* p, unsigned short fiu, unsigned short fip) {
	pushrect push;
	auto& f = p->get(fiu);
	width = f.sx;
	height = f.sy;
	auto pressed = ishilite() && hot.pressed;
	if(gui.disabled)
		pressed = false;
	image(p, pressed ? fip : fiu, 0);
}

static void allow_disable_button() {
	if(gui.disabled)
		return;
	if(gui.data.iskind<script>()) {
		auto p = bsdata<script>::elements + gui.data.value;
		if(p->test && !p->test(gui.value))
			gui.disabled = true;
	}
}

static void pressed_button() {
	button(gui.res, gui.frames[gui.disabled ? 3 : gui.checked ? 2 : 0], gui.frames[1]);
}

static void pressed_text() {
	auto push_caret = caret;
	auto push_fore = fore;
	if(gui.disabled)
		fore = fore.mix(colors::black);
	else if(hot.pressed && gui.hilited) {
		caret.x += 1;
		caret.y += 2;
	}
	texta(getname(), AlignCenterCenter);
	fore = push_fore;
	caret = push_caret;
}

static void button() {
	allow_disable_button();
	pressed_button();
	button_input();
	pressed_text();
}

static void button_no_text() {
	allow_disable_button();
	pressed_button();
	button_input();
}

static void button_animated() {
	gui.frames[0] = (unsigned)((current_game_tick/120) % gui.res->count);
	gui.frames[1] = gui.frames[0];
	button();
}

static void textarea() {
	auto push_font = font;
	if(gui.res)
		font = gui.res;
	textf(getname());
	font = push_font;
}

static void label() {
	auto push_font = font;
	auto push_fore = fore;
	if(gui.res)
		font = gui.res;
	if(gui.checked)
		fore = colors::special;
	switch(gui.frames[1]) {
	case 1: texta(getname(), AlignLeftCenter); break;
	case 2: texta(getname(), AlignRightCenter); break;
	default: texta(getname(), AlignCenterCenter); break;
	}
	font = push_font;
	fore = push_fore;
}

static void label_left() {
	auto push_font = font;
	auto push_fore = fore;
	if(gui.res)
		font = gui.res;
	if(gui.checked)
		fore = colors::special;
	caret.y += (height - texth()) / 2;
	text(getname());
	font = push_font;
	fore = push_fore;
}

static void pressed_colorgrad(int index, int size) {
	auto push_caret = caret;
	if(hot.pressed && gui.hilited) {
		caret.x += 1 + size;
		caret.y += 1;
	}
	auto push_palt = palt; palt = pallette;
	set_color(pallette, 4, index);
	image(gres(COLGRAD), size, ImagePallette);
	palt = push_palt;
	caret = push_caret;
}

static void color_picker() {
	auto color_index = color_indecies[gui.value];
	pressed_button();
	if(color_index == -1)
		return;
	pressed_colorgrad(color_index, 0);
	button_input();
}

static void creature_color() {
	auto color_index = player->colors[gui.value];
	pressed_button();
	if(color_index == -1)
		return;
	pressed_colorgrad(color_index, 1);
	button_input();
}

static void scroll() {
	if(!gui.res || !last_scroll)
		return;
	auto push_hilited = gui.hilited;
	auto& f = gui.res->get(gui.frames[1]);
	auto w = f.sx;
	auto h = f.sy;
	auto sh = gui.res->get(gui.frames[4]).sy;
	auto push_caret = caret;
	gui.hilited = ishilite({caret.x, caret.y, caret.x + w, caret.y + h});
	button(gui.res, gui.frames[1], gui.frames[0]);
	button_run_input();
	if(button_run)
		last_scroll->view_up();
	caret.y = push_caret.y + height - h;
	gui.hilited = ishilite({caret.x, caret.y, caret.x + w, caret.y + h});
	button(gui.res, gui.frames[3], gui.frames[2]);
	button_run_input();
	if(button_run)
		last_scroll->view_down();
	caret.y = push_caret.y + h;
	auto height_max = height - h * 2 - sh * 2;
	auto current_position = last_scroll->proportial(height_max);
	caret.y += current_position;
	button(gui.res, gui.frames[4], gui.frames[5]);
	button_run_input();
	caret = push_caret;
}

static void paint_draggable() {
	if(drag_item_source) {
		if(hot.key == MouseRight
			|| hot.key == KeyEscape
			|| (hot.key == MouseLeft && hot.pressed))
			execute(buttoncancel);
	}
	drag_item_dest = 0;
	last_form->paint();
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
	if(drag_item_source) {
		if(gui.hilited) {
			drag_item_dest = pi;
			image(gui.res, 25, 0);
		}
	}
}

static void input_item_info(const item* pi) {
	if(!drag_item_source) {
		if(gui.hilited && hot.key == MouseRight && hot.pressed)
			execute(open_item_description, 0, 0, pi);
	}
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

static void portrait_large() {
	image(gres(PORTL), player->portrait, 0);
}

static void number() {
	auto push_font = font;
	if(gui.res)
		font = gui.res;
	char temp[64]; stringbuilder sb(temp);
	sb.add("%1i", gui.value);
	texta(temp, AlignCenterCenter);
	font = push_font;
}

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

static void format_labelv(const char* format, const char* format_param) {
	char temp[260]; stringbuilder sb(temp);
	sb.addv(format, format_param);
	gui.text = temp;
	label();
}

static void format_label(const char* format, ...) {
	XVA_FORMAT(format);
	format_labelv(format, format_param);
}

static void player_coins() {
	if(!player)
		return;
	format_label("%1i", player->coins);
}

static void player_name() {
	if(!player)
		return;
	gui.text = player->getname();
	label();
}

static void store_name() {
	if(!last_store)
		return;
	gui.text = last_store->getname();
	label();
}

static void list_item_cost() {
}

static void list_item_price() {
}

static void item_name() {
	gui.text = last_item->getname();
	label();
}

static void buy_item_cost() {
	if(!store_item)
		return;
	format_label("%1i", store_item->getcost());
}

static void sell_item_cost() {
	if(!party_item)
		return;
	format_label("%1i", party_item->getcost());
}

static void paint_form() {
	if(gui.data.iskind<form>())
		bsdata<form>::elements[gui.data.value].paint();
	else if(gui.data.iskind<script>())
		bsdata<script>::elements[gui.data.value].proc(gui.data.value);
}

static void run_script() {
	if(gui.data.iskind<script>())
		bsdata<script>::elements[gui.data.value].proc(gui.data.value);
}

static void button_check(int& value) {
	gui.checked = (value == gui.value);
	pressed_button();
	auto run = (hot.key == MouseLeft && !hot.pressed && gui.hilited);
	if(hot.key == ('1' + gui.value))
		run = true;
	if(run)
		execute(set_value_and_update, gui.value, 0, &value);
}

static void button_info_tab() {
	button_check(current_info_tab);
}

//static void text_description() {
//	auto push_font = font;
//	if(gui.res)
//		font = gui.res;
//	area_description.paint(description_text);
//	if(gui.hilited)
//		area_description.input();
//	font = push_font;
//}

static void text_console() {
	auto push_font = font;
	if(gui.res)
		font = gui.res;
	area_console.paint((char*)console_data.data);
	if(gui.hilited)
		area_console.input();
	font = push_font;
}

static void set_ptr_and_invalidate() {
	cbsetptr();
	invalidate_description();
}


static void list_elements(void** current_focus) {
	if(!gui.data.iskind<listi>())
		return;
	auto ps = bsdata<listi>::elements + gui.data.value;
	if(!ps->elements.size())
		return;
	if(!(*current_focus))
		*current_focus = ps->elements.begin()->getpointer();
	auto max_height = caret.y + draw::height;
	draw::height = texth() + 2;
	for(auto v : ps->elements) {
		if(caret.y >= max_height)
			break;
		auto p = v.getpointer();
		gui.text = v.getname();
		gui.checked = (p == *current_focus);
		gui.hilited = ishilite();
		label_left();
		button_run_input();
		if(button_run)
			execute(set_ptr_and_invalidate, (long)p, 0, current_focus);
		caret.y += draw::height;
	}
}

static void list_elements(void** current_focus, const array& source) {
	if(!last_scroll)
		return;
	last_scroll->perscreen = height;
	last_scroll->perline = texth() + 2;
	last_scroll->maximum = (source.getcount() + 1) * last_scroll->perline + 1;
	last_scroll->correct();
	if(gui.hilited)
		last_scroll->input();
	if(!(*current_focus))
		*current_focus = source.begin();
	auto push_clip = clipping;
	setclipall();
	auto size = source.element_size;
	auto p = source.begin() + size * (last_scroll->origin / last_scroll->perline);
	auto pe = source.end();
	auto max_height = caret.y + draw::height;
	draw::height = last_scroll->perline;
	while(p < pe) {
		if(caret.y >= max_height)
			break;
		gui.text = ((nameable*)p)->getname();
		gui.checked = (p == *current_focus);
		gui.hilited = ishilite();
		label_left();
		button_run_input();
		if(button_run)
			execute(set_ptr_and_invalidate, (long)p, 0, current_focus);
		caret.y += last_scroll->perline;
		p += size;
	}
	clipping = push_clip;
}

static void list_collection(void** current_focus, const collectiona& source, scrollable& scroll) {
	last_scroll = &scroll;
	last_scroll->perline = 1;
	last_scroll->perscreen = gui.value;
	last_scroll->maximum = source.getcount();
	if(gui.hilited)
		last_scroll->input();
	if(!(*current_focus))
		*current_focus = source[0];
}

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

static itemlist::element* get_element() {
	if(!last_scroll)
		return 0;
	auto n = gui.value + last_scroll->origin;
	if(n >= last_scroll->maximum)
		return 0;
	itemlist* pn = static_cast<itemlist*>(last_scroll);
	return pn->data + n;
}

static void store_item_avatar() {
	auto pi = get_element();
	if(!pi)
		return;
	auto index = gui.value % 8;
	if(index >= 4)
		index += 4;
	image(gui.res, index, 0);
	if(gui.hilited) {
		switch(hot.key) {
		case MouseLeft:
			if(hot.pressed)
				execute(cbsetint, pi->count + 1, 0, &pi->count);
			break;
		case MouseLeftDBL:
			break;
		case MouseRight:
			if(pi->count > 0 && hot.pressed)
				execute(cbsetint, pi->count - 1, 0, &pi->count);
			break;
		}
	}
	//if(*pi->data)
	//	paint_item(pi->data);
	if(pi->count > 0)
		image(gui.res, 25, 0);
}

static void store_item_info() {
	auto pi = get_element();
	if(!pi)
		return;
	format_label("%1 - %2i%3", pi->data->getname(), pi->data->getcost(), getnm("GP"));
	input_item_info(pi->data);
}

static void topic_list() {
	static scrolllist scroll; last_scroll = &scroll;
	list_elements(&current_topic);
}

static void content_list() {
	auto p = (varianti*)current_topic;
	if(!p)
		return;
	static scrolllist scroll; last_scroll = &scroll;
	list_elements(&current_focus, *p->source);
}

void util_items_list();

void item_list_total(stringbuilder& sb) {
	auto p = get_item_list();
	if(!p)
		return;
	sb.add("%1i", p->maximum);
}

BSDATA(widget) = {
	{"ActionPanelNA", paint_action_panel},
	{"AreaMinimap", paint_minimap},
	{"AreaMap", paint_area},
	{"Background", background},
	{"Button", button},
	{"ButtonAM", button_animated},
	{"ButtonInfoTab", button_info_tab},
	{"ButtonNT", button_no_text},
	{"ColorPicker", color_picker},
	{"ContentList", content_list},
	{"CreatureColor", creature_color},
	{"TextConsole", text_console},
	{"Form", paint_form},
	{"BuyItemCost", buy_item_cost},
	{"HotKey", button_input},
	{"Label", label},
	{"ListItemCost", list_item_cost},
	{"ListItemPrice", list_item_price},
	{"Paperdoll", paperdoll},
	{"PlayerCoins", player_coins},
	{"PlayerName", player_name},
	{"PlayerItemList", player_items},
	{"PortraitLarge", portrait_large},
	{"Rectangle", rectb},
	{"RunScript", run_script},
	{"Scroll", scroll},
	{"SellItemCost", sell_item_cost},
	{"StoreItemAvatar", store_item_avatar},
	{"StoreItemInfo", store_item_info},
	{"StoreList", store_list},
	{"StoreName", store_name},
	{"TopicList", topic_list},
#ifdef _DEBUG
	{"ItemList", util_items_list},
#endif // _DEBUG
};
BSDATAF(widget)