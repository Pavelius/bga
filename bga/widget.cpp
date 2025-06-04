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

static void paint_empthy_weapon() {
	if(gui.value == player->weapon_index)
		image(gres(ITEMS), 0, 0);
	else
		image(gres(STON), 17, 0);
}

static void paint_empthy_offhand() {
	image(gres(STON), 13, 0);
}

static void paint_empthy_gear() {
	image(gres(STON), gui.value, 0);
}

static void quick_weapon_button() {
	gui.checked = (gui.value == player->weapon_index);
	button_no_text();
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

static void layer(color v) {
	auto push_alpha = alpha; alpha = 26;
	auto push_fore = fore; fore = v;
	rectf();
	alpha = push_alpha;
	fore = push_fore;
}

static point minimap_origin, minimap_size;

static point m2mm(point mm) {
	mm.x = mm.x / 8;
	mm.y = mm.y / 8;
	return minimap_origin + mm;
}

static point mm2m(point m) {
	m = m - minimap_origin;
	m.x *= 8;
	m.y *= 8;
	return m;
}

static void set_camera() {
	setcamera({(short)hot.param, (short)hot.param2});
}

static void paint_minimap() {
	if(last_screen.x2 == 0) {
		last_screen.x2 = 800;
		last_screen.y2 = 433;
	}
	pushrect push;
	// Minimap image
	auto mm = map::getminimap();
	auto& sf = mm->get(0);
	caret.x += (width - sf.sx) / 2;
	caret.y += (height - sf.sy) / 2;
	width = sf.sx; height = sf.sy;
	minimap_origin = caret;
	minimap_size.x = sf.sx;
	minimap_size.y = sf.sy;
	image(mm, 0, 0);
	cursor = default_cursor;
	if(ishilite()) {
		cursor.cicle = 44;
		if(hot.key == MouseLeft && hot.pressed) {
			auto np = mm2m(hot.mouse);
			execute(set_camera, np.x, np.y, 0);
		}
	}
	// Screen rect
	caret = m2mm(camera);
	point cameral = camera;
	cameral.x += last_screen.width();
	cameral.y += last_screen.height();
	cameral = m2mm(cameral);
	width = cameral.x - caret.x;
	height = cameral.y - caret.y;
	rectb();
	// Party position
	auto push_fore = fore;
	fore = colors::green;
	for(auto p : party) {
		if(!p->ispresent())
			continue;
		caret = m2mm(p->position);
		circle(2);
	}
	fore = push_fore;
}

static void input_item_info(const item* pi) {
	if(!drag_item_source) {
		if(gui.hilited && hot.key == MouseRight && hot.pressed)
			execute(open_item_description, 0, 0, pi);
	}
}

static void paint_item(const item* pi) {
	if(!pi)
		return;
	auto push_caret = caret;
	setoffset(2, 2);
	if(!player->isusable(*pi))
		layer(colors::red);
	image(gres(ITEMS), pi->geti().avatar * 2, 0);
	caret = push_caret;
	input_item_info(pi);
}

static void paint_item_dragable(item* pi) {
	paint_item(pi);
	if(gui.hilited && !drag_item_source) {
		if(hot.key == MouseLeft && hot.pressed)
			execute(begin_drag_item, 0, 0, pi);
	}
}

static void backpack_button() {
	auto pi = player->wears + Backpack + gui.value;
	auto index = gui.value % 8;
	if(index >= 4)
		index += 4;
	image(gui.res, index, 0);
	allow_drop_target(pi, Backpack);
	if(*pi)
		paint_item_dragable(pi);
}

static void backpack_image() {
	auto push_caret = caret;
	caret.x += 24;
	image(gui.res, gui.frames[0], 0);
}

static void paint_drop_target(item* pi, wear_s slot) {
	if(drag_item_source && drag_item.canequip(slot)) {
		if(player->isusable(drag_item)) {
			if(gui.hilited) {
				image(gui.res, 25, 0);
				drag_item_dest = pi;
			} else
				image(gui.res, 16, 0);
		}
	}
}

static void gear_button() {
	auto type = (wear_s)(Head + gui.value);
	auto pi = player->wears + type;
	paint_drop_target(pi, type);
	if(*pi)
		paint_item_dragable(pi);
	else
		strokeout(paint_empthy_gear, -2);
}

static void paint_empthy_quiver() {
	image(gres(STON), 11, 0);
}

static void quiver_button() {
	auto pi = player->wears + Quiver + gui.value;
	image(gui.res, gui.value, 0);
	paint_drop_target(pi, Quiver);
	if(*pi)
		paint_item_dragable(pi);
	else
		strokeout(paint_empthy_quiver, -2);
}

static void quick_item_button() {
	auto pi = player->wears + QuickItem + gui.value;
	image(gui.res, gui.value, 0);
	paint_drop_target(pi, QuickItem);
	if(*pi)
		paint_item_dragable(pi);
}

static void quick_weapon_item() {
	auto pi = player->wears + (QuickWeapon + gui.value * 2);
	image(gui.res, gui.value, 0);
	paint_drop_target(pi, QuickWeapon);
	if(!(*pi))
		strokeout(paint_empthy_weapon, -2);
	else
		paint_item_dragable(pi);
	if(!drag_item_source) {
		if(gui.value == player->weapon_index)
			image(gres(STONSLOT), 34, 0);
	}
}

static void quick_offhand_item() {
	auto pi = player->wears + (QuickOffhand + gui.value * 2);
	image(gui.res, 8 + gui.value, 0);
	paint_drop_target(pi, QuickOffhand);
	if(!(*pi))
		strokeout(paint_empthy_offhand, -2);
	else
		paint_item_dragable(pi);
	if(!drag_item_source && pi && *pi && player->useoffhand()) {
		if(gui.value == player->weapon_index)
			image(gres(STONSLOT), 34, 0);
	}
}

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

static void creature_ability() {
	if(gui.data.iskind<abilityi>())
		gui.value = player->get((ability_s)gui.data.value);
	else if(equal(gui.id, "HPMax"))
		gui.value = player->hp_max;
	else if(equal(gui.id, "HP"))
		gui.value = player->hp;
	else if(equal(gui.id, "Coins"))
		gui.value = player->coins;
	number();
}

static void creature_ability_bonus() {
	if(gui.data.iskind<abilityi>())
		gui.value = player->getbonus((ability_s)gui.data.value);
	char temp[32]; stringbuilder sb(temp);
	auto push_fore = fore;
	if(gui.value > 0) {
		sb.add("%+1i", gui.value);
		fore = colors::green;
	} else if(gui.value < 0) {
		fore = colors::red;
		sb.add("%+1i", gui.value);
	} else
		sb.add("%+1i", gui.value);
	gui.text = temp;
	label();
	fore = push_fore;
}

static void creature_race() {
	gui.text = bsdata<racei>::elements[player->race].getname();
	label();
}

static void apply_weight_color() {
	switch(player->getencumbrance()) {
	case 3: case 2: fore = colors::red; break;
	case 1: fore = colors::yellow; break;
	default: break;
	}
}

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

static void creature_weight() {
	char temp[260]; stringbuilder sb(temp);
	sb.add(getkg(player->weight));
	sb.adds("%-1", getnm("From"));
	sb.adds(getkg(player->allowed_weight));
	gui.text = temp;
	apply_weight_color();
	label();
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
	if(*pi->data)
		paint_item(pi->data);
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

static void enter_current_world_area() {
	auto p = (worldmapi::area*)hot.object;
	if(p)
		enter(p->id, 0);
}

static void paint_worldmap() {
	current_world_area_hilite = 0;
	if(!current_world)
		return;
	auto push_clip = clipping; setclipall();
	auto back = current_world->background->get();
	image(caret.x, caret.y, back, 0, 0);
	auto icons = current_world->icons->get();
	if(!icons)
		return;
	cursor = default_cursor;
	if(gui.hilited)
		cursor.cicle = 44;
	auto push_caret = caret;
	auto current_party_area = get_party_world_area();
	for(auto& e : bsdata<worldmapi::area>()) {
		if(e.realm != current_world)
			continue;
		//if(!e.is(AreaVisible))
		//	continue;
		caret = push_caret + e.position;
		auto& f = icons->get(e.avatar);
		caret.x -= f.sx / 2;
		caret.y -= f.sy / 2;
		image(icons, e.avatar, 0);
		if(current_party_area == &e)
			image(icons, 22, 0);
		fore = colors::white;
		if(e.isinteract()) {
			if(hot.mouse.in({caret.x - 2, caret.y - 2, caret.x + f.sx + 2, caret.y + f.sy + texth() + 2}))
				current_world_area_hilite = &e;
		} else
			fore = fore.mix(colors::black, 128);
		if(current_world_area_hilite == &e) {
			cursor.cicle = 34;
			fore = colors::yellow;
			if(hot.key == MouseLeft && !hot.pressed)
				execute(enter_current_world_area, 0, 0, current_world_area_hilite);
		}
		auto name = e.getname();
		auto w = textw(name);
		caret.x -= (w - f.sx) / 2;
		caret.y += f.sy;
		text(name, -1, TextStroke);
	}
	caret = push_caret;
	clipping = push_clip;
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
	{"BackpackButton", backpack_button},
	{"BackpackImage", backpack_image},
	{"Button", button},
	{"ButtonAM", button_animated},
	{"ButtonInfoTab", button_info_tab},
	{"ButtonNT", button_no_text},
	{"ColorPicker", color_picker},
	{"ContentList", content_list},
	{"CreatureAbility", creature_ability},
	{"CreatureAbilityBonus", creature_ability_bonus},
	{"CreatureColor", creature_color},
	{"CreatureRace", creature_race},
	{"CreatureWeight", creature_weight},
	{"TextConsole", text_console},
	{"Form", paint_form},
	{"BuyItemCost", buy_item_cost},
	{"GearButton", gear_button},
	{"HotKey", button_input},
	{"Label", label},
	{"ListItemCost", list_item_cost},
	{"ListItemPrice", list_item_price},
	{"Paperdoll", paperdoll},
	{"PlayerCoins", player_coins},
	{"PlayerName", player_name},
	{"PlayerItemList", player_items},
	{"PortraitLarge", portrait_large},
	{"QuickItemButton", quick_item_button},
	{"QuickWeaponButton", quick_weapon_button},
	{"QuickWeaponItem", quick_weapon_item},
	{"QuickOffhandItem", quick_offhand_item},
	{"QuiverButton", quiver_button},
	{"Rectangle", rectb},
	{"RunScript", run_script},
	{"Scroll", scroll},
	{"SellItemCost", sell_item_cost},
	{"StoreItemAvatar", store_item_avatar},
	{"StoreItemInfo", store_item_info},
	{"StoreList", store_list},
	{"StoreName", store_name},
	{"TopicList", topic_list},
	{"Worldmap", paint_worldmap},
#ifdef _DEBUG
	{"ItemList", util_items_list},
#endif // _DEBUG
};
BSDATAF(widget)