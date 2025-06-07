#include "creature.h"
#include "draw.h"
#include "pushvalue.h"
#include "resid.h"
#include "store.h"
#include "vector.h"
#include "view.h"
#include "view_list.h"

using namespace draw;

struct tradegood {
	item* source;
	int count;
};
struct tradegooda : vector<tradegood> {
	int total() const;
	void add(item& e);
	bool checked() const;
};

static tradegooda shop_goods, player_goods;

int tradegooda::total() const {
	auto result = 0;
	for(auto& e : *this)
		result += e.count * e.source->getcost();
	return result;
}

bool tradegooda::checked() const {
	for(auto& e : *this) {
		if(e.count)
			return true;
	}
	return false;
}

void tradegooda::add(item& e) {
	auto p = vector<tradegood>::add();
	p->source = &e;
	p->count = 0;
}

static void update_player_items() {
	player_goods.clear();
	for(auto& e : player->backpack()) {
		if(!e)
			continue;
		player_goods.add(e);
	}
}

static void update_items() {
	update_player_items();
}

static void pick_good() {
	auto p = (tradegood*)hot.object;
	if(!p->count)
		p->count = p->source->count;
	else
		p->count = 0;
}

static void remove_good() {
	auto p = (tradegood*)hot.object;
	p->count = 0;
}

static void paint_good(void* object) {
	pushrect push;
	pushfore push_fore;
	auto p = (tradegood*)object;
	//if(hot.mouse.in({caret.x, caret.y, caret.x + 32, caret.y + 32}) && hot.key == MouseRight && !hot.pressed)
	//	execute(remove_good, 0, 0, object);
	if(p->count)
		image(gres(STONSLOT), 25, 0);
	paint_item(p->source);
	caret.x += 50; caret.y += 1; width = 160; height = 34;
	auto push_clip = clipping; setclipall();
	text(str("%1", p->source->getname())); caret.y += texth();
	fore = fore.mix(colors::gray, 64);
	text(str("%1i gp", p->source->getcostall()));
	clipping = push_clip;
}

static void paint_player_goods() {
	static int origin;
	setdialog(401, 113, 214, 264);
	paint_list(player_goods.data, player_goods.element_size, player_goods.count, origin, 6, paint_good, 45, {10, 1}, -4, pick_good, 0);
	//setdialog(401, 113); button(GUISTMSC, 1, 2);
	//setdialog(450, 116, 164, 30); texta(NORMAL, "Store", AlignCenterCenter);
}

static void paint_store() {
	paint_game_dialog(GUISTDRB);
	auto player_total = player_goods.total();
	setdialog(134, 23, 238, 28); texta(STONEBIG, "Buy and Sell", AlignCenterCenter);
	setdialog(663, 191); button(GBTNSTD, 1, 2, 0, "Buy", 3, false);
	setdialog(663, 220); button(GBTNSTD, 1, 2, 0, "Sell", 3, player_total);
	// setdialog(663, 249); button(GBTNSTD, 1, 2);
	setdialog(663, 384); button(GBTNSTD, 1, 2, KeyEscape, "Done"); fire(buttoncancel);
	setdialog(400, 23, 238, 28); texta(STONEBIG, "Store", AlignCenterCenter);
	paint_player_goods();
	setdialog(135, 113); button(GUISTMSC, 1, 2);
	setdialog(184, 116, 164, 30); texta(NORMAL, "Store", AlignCenterCenter);
	setdialog(692, 90, 80, 20); texta(NORMAL, "Store", AlignCenterCenter);
	setdialog(285, 387, 80, 20); texta(NORMAL, "Store", AlignCenterCenter);
	setdialog(141, 83, 225, 18); texta(NORMAL, "Store", AlignCenterCenter);
	setdialog(407, 83, 225, 18); texta(player->getname(), AlignCenterCenter);
	setdialog(138, 387, 125, 20); texta(getnm("Cost"), AlignRightCenter);
	setdialog(403, 387, 125, 20); texta(getnm("Price"), AlignRightCenter);
	setdialog(551, 387, 80, 20); texta(str("%1i", player_total), AlignCenterCenter);
	// setdialog(668, 123); button(CONTBACK, 0, 1);
	paint_action_panel_player();
}

void open_store() {
	pushvalue push_change(on_player_change, update_items);
	last_store = bsdata<storei>::elements;
	update_items();
	scene(paint_store);
}