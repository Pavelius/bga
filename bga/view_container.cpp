#include "action.h"
#include "area.h"
#include "console.h"
#include "container.h"
#include "creaturea.h"
#include "draw.h"
#include "drawable.h"
#include "game.h"
#include "itemground.h"
#include "keybind.h"
#include "math.h"
#include "pushvalue.h"
#include "rfiles.h"
#include "timer.h"
#include "vector.h"
#include "view.h"
#include "view_list.h"

using namespace draw;

static vector<item*> container_items, items;

static int container_frame(container::typen type) {
	switch(type) {
	case container::Bag: return 1;
	case container::Barrel: return 3;
	case container::Altar: return 0;
	case container::Body: return 4;
	case container::Spellbook: return 5;
	default: return 0;
	}
}

static int container_frame() {
	if(!last_container)
		return 10;
	return container_frame(last_container->type);
}

static void pick_container_item() {
	need_update_items = true;
	auto p = (item*)hot.object;
	player->additem(*p);
}

static void pick_player_item() {
	need_update_items = true;
	auto p = (item*)hot.object;
	if(last_container)
		last_container->add(*p);
	else
		add_item(current_area, player->position, *p);
}

static void view_container_item(void* object) {
	pushrect push;
	auto p = (item*)object;
	paint_item(p);
}

static void view_player_item(void* object) {
	pushrect push;
	auto p = (item*)object;
	paint_item(p);
}

static void view_container_items() {
	static int origin;
	paint_list(container_items.data, 0, container_items.count, origin, 2, 5,
		view_container_item, 45, 44, {5, 3}, -14, pick_container_item, 0);
}

static void view_player_items() {
	static int origin;
	paint_list(items.data, 0, items.count, origin, 2, 2,
		view_player_item, 45, 44, {5, 3}, -14, pick_player_item, 0);
}

static void select_items(vector<item*>& source, short unsigned area, point position) {
	for(auto& e : bsdata<itemground>()) {
		if(e.area != area)
			continue;
		if(e && e.position == position)
			source.add(&e);
	}
}

static void select_player_backpack() {
	if(!player)
		return;
	items.clear();
	for(auto& e : player->backpack()) {
		if(e)
			items.add(&e);
	}
}

static void select_container_items() {
	if(!last_container)
		return;
	container_items.clear();
	select_items(container_items, current_area, {getbsi(last_container), itemground::Container});
}

static void update_items() {
	if(need_update_items) {
		need_update_items = false;
		select_container_items();
		select_player_backpack();
	}
}

static void update_ground_items() {
	if(need_update_items) {
		need_update_items = false;
		container_items.clear();
		select_items(container_items, current_area, player->position);
		select_player_backpack();
	}
}

static void view_pick_container() {
	auto pc = gres("CONTAINER");
	paint_game_dialog(0, 476, "GUICONT", 1);
	setdialog(60, 25); image(pc, container_frame(), 0);
	setdialog(430, 28); image(pc, 1, 0);
	setdialog(150, 22); stoneslot(0, 0);
	setdialog(195, 22); stoneslot(0, 0);
	setdialog(240, 22); stoneslot(0, 0);
	setdialog(285, 22); stoneslot(0, 0);
	setdialog(330, 22); stoneslot(0, 0);
	setdialog(150, 65); stoneslot(0, 0);
	setdialog(195, 65); stoneslot(0, 1);
	setdialog(240, 65); stoneslot(0, 1);
	setdialog(285, 65); stoneslot(0, 1);
	setdialog(330, 65); stoneslot(0, 1);
	setdialog(148, 21, 222, 90); view_container_items();
	setdialog(510, 22); stoneslot(0, 0);
	setdialog(555, 22); stoneslot(0, 0);
	setdialog(510, 65); stoneslot(0, 0);
	setdialog(555, 65); stoneslot(0, 0);
	setdialog(508, 21, 90, 90); view_player_items();
	setdialog(661, 78, 70, 20); texta(str("%1i", player->coins), AlignRightCenter);
	setdialog(684, 28); button(pma_butopt1, 1, 2, KeyEscape); fire(buttoncancel);
}

static void mouse_area_cancel() {
	if(hot.key == MouseLeft && !hot.pressed && ishilite())
		breakmodal(0);
}

static void paint_container_area() {
	update_frames();
	update_items();
	setcaret(0, 0, 800, 476);
	mouse_area_cancel();
	paint_area_no_keys();
	view_pick_container();
}

void open_container() {
	if(hot.param2 == 0xFFFF)
		return;
	pushvalue push_player(player, (creature*)hot.param);
	pushvalue push_container(last_container, (container*)bsdata<container>::elements + hot.param2);
	need_update_items = true;
	scene(paint_container_area);
}

static void view_ground_item_area() {
	update_frames();
	update_ground_items();
	setcaret(0, 0, 800, 476);
	mouse_area_cancel();
	paint_area_no_keys();
	view_pick_container();
}

void open_ground_items() {
	if(hot.param2 == 0xFFFF)
		return;
	pushvalue push_player(player, (creature*)hot.param);
	pushvalue push_container(last_container, (container*)0);
	need_update_items = true;
	scene(view_ground_item_area);
}