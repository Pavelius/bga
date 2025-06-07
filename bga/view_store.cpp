#include "creature.h"
#include "draw.h"
#include "iteminside.h"
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
static int trade_mode;

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

static void update_shop_items() {
	variant parent = last_store;
	shop_goods.clear();
	if(!last_store->is(UserAllowBuy))
		return;
	for(auto& e : bsdata<iteminside>()) {
		if(e.parent == parent)
			shop_goods.add(e);
	}
}

static void update_player_items() {
	player_goods.clear();
	if(!last_store->is(UserAllowSell))
		return;
	for(auto& e : player->backpack()) {
		if(!e)
			continue;
		player_goods.add(e);
	}
}

static void update_items() {
	update_player_items();
	update_shop_items();
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
	height = 34;
	//if(hot.mouse.in({caret.x, caret.y, caret.x + 32, caret.y + 32}) && hot.key == MouseRight && !hot.pressed)
	//	execute(remove_good, 0, 0, object);
	if(p->count)
		image(gres(STONSLOT), 25, 0);
	paint_item(p->source);
	caret.x += 50; caret.y += 1; width = 160;
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
}

static void paint_shop_goods() {
	static int origin;
	setdialog(135, 113, 214, 264);
	paint_list(shop_goods.data, shop_goods.element_size, shop_goods.count, origin, 6, paint_good, 45, {10, 1}, -4, pick_good, 0);
}

static void paint_right_panel() {
	setdialog(23, 78);
	if(last_store->is(UserAllowSell) || last_store->is(UserAllowBuy)) {
		checkbox(trade_mode, UserAllowBuy, GUISTBBC, 1, 2, 0, 0);
		caret.y += 80;
	}
	if(last_store->is(UserAllowIdentify)) {
		checkbox(trade_mode, UserAllowIdentify, GUISTBBC, 4, 5, 3, 0);
		caret.y += 80;
	}
	if(last_store->is(UserPurchaseCures)) {
		checkbox(trade_mode, UserPurchaseCures, GUISTBBC, 10, 11, 9, 0);
		caret.y += 80;
	}
	if(last_store->is(UserPurchaseDrinks)) {
		checkbox(trade_mode, UserPurchaseDrinks, GUISTBBC, 16, 17, 15, 0);
		caret.y += 80;
	}
	if(last_store->is(AllowPeasantRoom) || last_store->is(AllowMerchantRoom)
		|| last_store->is(AllowNobleRoom) || last_store->is(AllowRoyalRoom)) {
		checkbox(trade_mode, AllowPeasantRoom, GUISTBBC, 19, 20, 18, 0);
		caret.y += 80;
	}
}

static int get_back_frame() {
	switch(trade_mode) {
	case AllowPeasantRoom: return 3;
	default: return 0;
	}
}

static void paint_buy_sell() {
	auto player_total = player_goods.total();
	auto shop_total = shop_goods.total();
	setdialog(134, 23, 238, 28); texta(STONEBIG, getnm("BuyAndSell"), AlignCenterCenter);
	setdialog(400, 23, 238, 28); texta(STONEBIG, last_store->getname(), AlignCenterCenter);
	setdialog(663, 191); button(GBTNSTD, 1, 2, 0, "Buy", 3, shop_total);
	setdialog(663, 220); button(GBTNSTD, 1, 2, 0, "Sell", 3, player_total);
	if(last_store->steal_difficult) {
		setdialog(663, 249); button(GBTNSTD, 1, 2, 0, "Steal", 3, shop_total);
	}
	setdialog(141, 83, 225, 18); texta(getnm("Store"), AlignCenterCenter);
	setdialog(407, 83, 225, 18); texta(player->getname(), AlignCenterCenter);
	paint_shop_goods();
	setdialog(138, 387, 125, 20); texta(getnm("Cost"), AlignRightCenter);
	setdialog(285, 387, 80, 20); texta(str("%1i", shop_total), AlignCenterCenter);
	paint_player_goods();
	setdialog(403, 387, 125, 20); texta(getnm("Price"), AlignRightCenter);
	setdialog(551, 387, 80, 20); texta(str("%1i", player_total), AlignCenterCenter);
	setdialog(692, 90, 80, 20); texta(str("%1i gp", player->coins), AlignRightCenter);
	setdialog(690, 123); image(gres(CONTAINER), 1, 0);
	setdialog(663, 384); button(GBTNSTD, 1, 2, KeyEscape, "Done"); fire(buttoncancel);
}

static void paint_inn() {
	setdialog(136, 114); image(gres(ROOMS), 3 * 2, 0);
	setdialog(259, 114); image(gres(ROOMS), 1 * 2, 0);
	setdialog(136, 250); image(gres(ROOMS), 0 * 2, 0);
	setdialog(259, 250); image(gres(ROOMS), 2 * 2, 0);
	setdialog(133, 216); button(GBTNSTD, 1, 2);
	setdialog(256, 216); button(GBTNSTD, 1, 2);
	setdialog(133, 352); button(GBTNSTD, 1, 2);
	setdialog(256, 352); button(GBTNSTD, 1, 2);
	setdialog(134, 23, 238, 28); texta(STONEBIG, "Rooms", AlignCenterCenter);
	setdialog(400, 23, 238, 28); texta(STONEBIG, "268435464", AlignCenterCenter);
	setdialog(692, 90, 80, 20); texta(NORMAL, "268435465", AlignCenterCenter);
	setdialog(663, 123); button(GBTNSTD, 1, 2);
	setdialog(404, 82, 209, 325); texta(NORMAL, "", AlignCenterCenter);
	//Scroll GBTNSCRL 625 81 12 327 frames(1 0 3 2 4 5)
	setdialog(285, 387, 80, 20); texta(NORMAL, "268435469", AlignCenterCenter);
	setdialog(138, 387, 125, 20); texta(NORMAL, "Cost", AlignCenterCenter);
	setdialog(141, 83, 225, 18); texta(NORMAL, "Room Quality", AlignCenterCenter);
}

static void paint_store() {
	auto player_total = player_goods.total();
	auto shop_total = shop_goods.total();
	paint_game_dialog(GUISTDRB, get_back_frame());
	paint_right_panel();
	switch(trade_mode) {
	case AllowPeasantRoom: paint_inn(); break;
	default: paint_buy_sell(); break;
	}
	paint_action_panel_player();
	paint_game_panel(false);
}

void open_store() {
	pushvalue push_change(on_player_change, update_items);
	last_store = bsdata<storei>::elements;
	update_items();
	scene(paint_store);
}