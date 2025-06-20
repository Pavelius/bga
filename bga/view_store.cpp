#include "creature.h"
#include "draw.h"
#include "game.h"
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
	int checkedcount() const;
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

int tradegooda::checkedcount() const {
	auto result = 0;
	for(auto& e : *this) {
		if(e.count)
			result++;
	}
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

static void pick_identify() {
	auto p = (tradegood*)hot.object;
	pushvalue push_item(last_item, p->source);
	pick_good();
	set_description("%ItemInformation");
}

static void remove_good() {
	auto p = (tradegood*)hot.object;
	p->count = 0;
}

static void paint_player_coins() {
	texta(str("%1i gp", player->coins), AlignRightCenter);
}

static void paint_store_name() {
	texta(STONEBIG, last_store->getname(), AlignCenterCenter);
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

static void paint_identify_item(void* object) {
	pushrect push;
	pushfore push_fore;
	auto p = (tradegood*)object;
	height = 34;
	if(p->count)
		image(gres(STONSLOT), 25, 0);
	paint_item(p->source);
	caret.x += 50; caret.y += 1; width = 160;
	texta(p->source->getname(), AlignCenterCenter);
}

static void paint_identify_items() {
	static int origin;
	paint_list(player_goods.data, player_goods.element_size, player_goods.count, origin, 6,
		paint_identify_item, 45, {10, 1}, -4, pick_identify, 0, false);
}

static void paint_player_goods() {
	static int origin;
	paint_list(player_goods.data, player_goods.element_size, player_goods.count, origin, 6,
		paint_good, 45, {10, 1}, -4, pick_good, 0, false);
}

static void paint_shop_goods() {
	static int origin;
	paint_list(shop_goods.data, shop_goods.element_size, shop_goods.count, origin, 6,
		paint_good, 45, {10, 1}, -4, pick_good, 0, false);
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
	case UserAllowIdentify: return 2;
	case UserPurchaseDrinks: return 1;
	default: return 0;
	}
}

static void sell_goods() {
	auto total = player_goods.total();
	for(auto& e : player_goods) {
		if(!e.count)
			continue;
		item it(e.source->type);
		it.setcount(e.count);
		e.source->setcount(e.source->count - e.count);
		add_item(last_store, it);
	}
	player->coins += total;
	update_items();
}

static void paint_buy_sell() {
	auto player_total = player_goods.total();
	auto shop_total = shop_goods.total();
	setdialog(134, 23, 238, 28); texta(STONEBIG, getnm("BuyAndSell"), AlignCenterCenter);
	setdialog(400, 23, 238, 28); paint_store_name();
	setdialog(663, 191); button(GBTNSTD, 1, 2, 0, "Buy", 3, shop_total);
	setdialog(663, 220); button(GBTNSTD, 1, 2, 0, "Sell", 3, player_total); fire(sell_goods);
	if(last_store->steal_difficult) {
		setdialog(663, 249); button(GBTNSTD, 1, 2, 0, "Steal", 3, shop_total);
	}
	setdialog(141, 83, 225, 18); texta(getnm("Store"), AlignCenterCenter);
	setdialog(407, 83, 225, 18); texta(player->getname(), AlignCenterCenter);
	setdialog(135, 113, 214, 264); paint_shop_goods();
	setdialog(138, 387, 125, 20); texta(getnm("Cost"), AlignRightCenter);
	setdialog(285, 387, 80, 20); texta(str("%1i", shop_total), AlignCenterCenter);
	setdialog(401, 113, 214, 264); paint_player_goods();
	setdialog(403, 387, 125, 20); texta(getnm("Price"), AlignRightCenter);
	setdialog(551, 387, 80, 20); texta(str("%1i", player_total), AlignCenterCenter);
	setdialog(692, 90, 80, 20); paint_player_coins();
	setdialog(690, 123); image(gres(CONTAINER), 1, 0);
	paint_action_panel_player();
}

static void checkroom(storefn v, int f1, const char* id) {
	auto allowed = last_store->is(v);
	if(!allowed)
		f1++;
	image(gres(ROOMS), f1, 0);
	caret.x -= 3; caret.y += 102;
	button(GBTNSTD, 1, 2, 0, id, 3, allowed);
}

static void paint_inn() {
	setdialog(134, 23, 238, 28); texta(STONEBIG, getnm("Rooms"), AlignCenterCenter);
	setdialog(400, 23, 238, 28); paint_store_name();
	setdialog(141, 83, 225, 18); texta(getnm("RoomQuality"), AlignCenterCenter);
	setdialog(136, 114); checkroom(AllowPeasantRoom, 0 * 2, "RoomPeasant");
	setdialog(259, 114); checkroom(AllowMerchantRoom, 1 * 2, "RoomMerchant");
	setdialog(136, 250); checkroom(AllowNobleRoom, 2 * 2, "RoomNoble");
	setdialog(259, 250); checkroom(AllowRoyalRoom, 3 * 2, "RoomRoyal");
	setdialog(692, 90, 80, 20); paint_player_coins();
	setdialog(663, 123); button(GBTNSTD, 1, 2);
	setdialog(404, 82, 209, 325); paint_description(12, -1, 2);
	setdialog(138, 387, 125, 20); texta(getnm("Cost"), AlignRightCenter);
	setdialog(285, 387, 80, 20); texta("268435469", AlignCenterCenter);
	paint_action_panel_na();
}

static void paint_drink() {
	setdialog(140, 117, 40, 20); texta(NORMAL, "100", AlignCenterCenter);
	setdialog(195, 111); button(GBTNMED2, 1, 2, 0, "Rumors");
	setdialog(199, 82, 170, 20); texta(getnm("Drink"), AlignCenterCenter);
	setdialog(137, 82, 48, 20); texta(getnm("Price"), AlignCenterCenter);
	setdialog(134, 23, 238, 28); texta(STONEBIG, getnm("Drinks"), AlignCenterCenter);
	setdialog(400, 23, 238, 28); paint_store_name();
	setdialog(692, 90, 80, 20); paint_player_coins();
	setdialog(403, 82, 232, 20); texta(getnm("Rumors"), AlignCenterCenter);
	setdialog(404, 115, 210, 292); paint_description(12, -1, 2);
	paint_action_panel_na();
}

static void paint_identify() {
	auto shop_total = player_goods.checkedcount() * game.get(IdentifyCost);
	setdialog(134, 23, 238, 28); texta(STONEBIG, getnm("Identifying"), AlignCenterCenter);
	setdialog(400, 23, 238, 28); paint_store_name();
	setdialog(692, 90, 80, 20); paint_player_coins();
	setdialog(138, 387, 125, 20); texta(getnm("Cost"), AlignRightCenter);
	setdialog(285, 387, 80, 20); texta(str("%1i", shop_total), AlignCenterCenter);
	setdialog(663, 123); button(GBTNSTD, 1, 2, 0, "Identify", 3, shop_total);
	setdialog(141, 83, 225, 18); texta(getnm("Items"), AlignCenterCenter);
	//Scroll GBTNSCRL 359 114 12 260 frames(1 0 3 2 4 5)
	setdialog(135, 113, 214, 264); paint_identify_items();
	setdialog(404, 82, 209, 325); paint_description(12, -1, 2);
	paint_action_panel_player();
}

static void paint_store() {
	auto player_total = player_goods.total();
	auto shop_total = shop_goods.total();
	paint_game_dialog(GUISTDRB, get_back_frame());
	paint_right_panel();
	switch(trade_mode) {
	case AllowPeasantRoom: paint_inn(); break;
	case UserPurchaseDrinks: paint_drink(); break;
	case UserAllowIdentify: paint_identify(); break;
	default: paint_buy_sell(); break;
	}
	setdialog(663, 384); button(GBTNSTD, 1, 2, KeyEscape, "Done"); fire(buttoncancel);
	paint_game_panel(false, false);
}

void open_store() {
	pushvalue push_change(on_player_change, update_items);
	last_store = bsdata<storei>::elements;
	update_items();
	scene(paint_store);
}