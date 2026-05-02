#include "audio.h"
#include "creature.h"
#include "draw.h"
#include "game.h"
#include "itemground.h"
#include "pushvalue.h"
#include "store.h"
#include "vector.h"
#include "view.h"
#include "view_list.h"

using namespace draw;

struct tradegood {
	item*	source;
	int		count;
};
struct tradegooda : vector<tradegood> {
	void add(item& e);
	bool checked() const;
	int	checkedcount() const;
	int	total() const;
};

static tradegooda shop_goods, player_goods;
static int trade_mode;
static int room_rent;

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
	shop_goods.clear();
	if(!last_store->is(UserAllowBuy))
		return;
	auto index = last_store->index();
	for(auto& e : bsdata<itemground>()) {
		if(e.inside(e.Store, index))
			shop_goods.add(e);
	}
}

static void update_player_items() {
	player_goods.clear();
	if(!last_store->is(UserAllowSell))
		return;
	if(!player)
		return;
	for(auto& e : player->backpack()) {
		if(!e)
			continue;
		player_goods.add(e);
	}
}

static void update_items() {
	if(need_update_items) {
		need_update_items = false;
		update_player_items();
		update_shop_items();
	}
}

static void pick_good() {
	auto p = (tradegood*)hot.object;
	if(!p->count) {
		if(p->source->count > 1) {
			pushvalue push(last_item);
			last_item = p->source;
			open_item_count();
			if(last_number) {
				if(last_number > p->source->count)
					last_number = p->source->count;
				p->count = last_number;
			}
		} else
			p->count = p->source->count;
	} else
		p->count = 0;
}

static void mark_good() {
	auto p = (tradegood*)hot.object;
	if(!p->count)
		p->count = p->source->count;
	else
		p->count = 0;
}

static void pick_identify() {
	mark_good();
	auto p = (tradegood*)hot.object;
	pushvalue push_item(last_item, p->source);
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
	texta(metrics::h1, last_store->getname(), AlignCenterCenter);
}

static void paint_good(void* object) {
	pushrect push;
	pushfore push_fore;
	auto p = (tradegood*)object;
	height = 34;
	//if(hot.mouse.in({caret.x, caret.y, caret.x + 32, caret.y + 32}) && hot.key == MouseRight && !hot.pressed)
	//	execute(remove_good, 0, 0, object);
	if(p->count)
		image(pma_stoneslot, 25, 0);
	paint_item(p->source, p->source->count, p->count);
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
		image(pma_stoneslot, 25, 0);
	paint_item(p->source);
	caret.x += 50; caret.y += 1; width = 160;
	texta(p->source->getname(), AlignCenterCenter);
}

static void paint_identify_items() {
	static int origin;
	paint_list(player_goods.data, player_goods.element_size, player_goods.count, origin, 6,
		paint_identify_item, 45, {10, 1}, -4, pick_identify, 0);
}

static void paint_player_goods() {
	static int origin;
	paint_list(player_goods.data, player_goods.element_size, player_goods.count, origin, 6,
		paint_good, 45, {10, 1}, -4, pick_good, 0);
}

static void paint_shop_goods() {
	static int origin;
	paint_list(shop_goods.data, shop_goods.element_size, shop_goods.count, origin, 6,
		paint_good, 45, {10, 1}, -4, pick_good, 0);
}

static void paint_right_panel() {
	auto pb1 = gres("GUISTBBC");
	setdialog(23, 78);
	if(last_store->is(UserAllowSell) || last_store->is(UserAllowBuy)) {
		checkbox(trade_mode, UserAllowBuy, pb1, 1, 2, 0, 0);
		if(button_executed)
			set_description_id("UserAllowBuy");
		caret.y += 80;
	}
	if(last_store->is(UserAllowIdentify)) {
		checkbox(trade_mode, UserAllowIdentify, pb1, 4, 5, 3, 0);
		if(button_executed)
			set_description_id("UserAllowIdentify");
		caret.y += 80;
	}
	if(last_store->is(UserPurchaseCures)) {
		checkbox(trade_mode, UserPurchaseCures, pb1, 10, 11, 9, 0);
		if(button_executed)
			set_description_id("UserPurchaseCures");
		caret.y += 80;
	}
	if(last_store->is(UserPurchaseDrinks)) {
		checkbox(trade_mode, UserPurchaseDrinks, pb1, 16, 17, 15, 0);
		if(button_executed)
			set_description_id("UserPurchaseDrinks");
		caret.y += 80;
	}
	if(last_store->is(AllowPeasantRoom) || last_store->is(AllowMerchantRoom)
		|| last_store->is(AllowNobleRoom) || last_store->is(AllowRoyalRoom)) {
		checkbox(trade_mode, AllowPeasantRoom, pb1, 19, 20, 18, 0);
		if(button_executed)
			set_description_id("UserAllowRentRooms");
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

static bool check_coins(int value) {
	if(creature::coins >= (unsigned)value)
		return true;
	play_sound("GAM_47");
	statusr("NotEnoughGoldCoins", value, creature::coins, value - creature::coins);
	return false;
}

static int checkout(tradegooda& source, storefn type) {
	switch(type) {
	case AllowPeasantRoom: case AllowMerchantRoom: case AllowNobleRoom: case AllowRoyalRoom:
		return last_store->getcost(type);
	case UserAllowIdentify:
		return source.checkedcount() * game.get(IdentifyCost) * last_store->getcost(type) / 100;
	default:
		return source.total() * last_store->getcost(type) / 100;
	}
}

static void buy_goods() {
	auto total = checkout(shop_goods, UserAllowBuy);
	if(!check_coins(total))
		return;
	for(auto& e : shop_goods) {
		if(!e.count)
			continue;
		item it(e.source->type, e.count);
		e.source->setcount(e.source->count - e.count);
		party_add_item(it);
	}
	player->coins -= total;
	need_update_items = true;
}

static void sell_goods() {
	auto total = checkout(player_goods, UserAllowSell);
	for(auto& e : player_goods) {
		if(!e.count)
			continue;
		item it(e.source->type, e.count);
		e.source->setcount(e.source->count - e.count);
		last_store->add(it);
	}
	player->coins += total;
	need_update_items = true;
}

static void identify_all() {
	auto total = checkout(player_goods, UserAllowIdentify);
	if(!check_coins(total))
		return;
	for(auto& e : player_goods) {
		if(!e.count)
			continue;
		e.source->identify(1);
	}
	player->coins -= total;
	need_update_items = true;
}

static void inn_rest() {
	auto total = checkout(player_goods, (storefn)room_rent);
	if(!check_coins(total))
		return;
	game_disappear();
	player->coins -= total;
	for(auto p : party) {
	}
	game_appear();
}

static void paint_buy_sell() {
	update_items();
	auto player_total = checkout(player_goods, UserAllowSell);
	auto shop_total = checkout(shop_goods, UserAllowBuy);
	setdialog(134, 23, 238, 28); texta(metrics::h1, getnm("BuyAndSell"), AlignCenterCenter);
	setdialog(400, 23, 238, 28); paint_store_name();
	setdialog(663, 191); button(pma_butstd, 1, 2, 0, "Buy", 3, shop_total); fire(buy_goods);
	setdialog(663, 220); button(pma_butstd, 1, 2, 0, "Sell", 3, player_total); fire(sell_goods);
	if(last_store->steal_difficult) {
		setdialog(663, 249); button(pma_butstd, 1, 2, 0, "Steal", 3, shop_total);
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
	setdialog(690, 123); image(gres("CONTAINER"), 1, 0);
	paint_action_panel_player();
}

static void checkroom(storefn v, int f1, const char* id) {
	auto allowed = last_store->is(v);
	if(!allowed)
		f1++;
	image(gres("ROOMS"), f1, 0);
	caret.x -= 3; caret.y += 102;
	if(allowed && room_rent == -1)
		room_rent = v;
	auto btn = 1;
	if(room_rent == v)
		btn = 0;
	button(pma_butstd, btn, 2, 0, id, 3, allowed);
	if(button_executed)
		set_description_id(bsdata<storefi>::elements[v].id);
	fire(cbsetintds, v, 0, &room_rent);
}

static void paint_inn() {
	setdialog(134, 23, 238, 28); texta(metrics::h1, getnm("Rooms"), AlignCenterCenter);
	setdialog(400, 23, 238, 28); paint_store_name();
	setdialog(141, 83, 225, 18); texta(getnm("RoomQuality"), AlignCenterCenter);
	setdialog(136, 114); checkroom(AllowPeasantRoom, 0 * 2, "RoomPeasant");
	setdialog(259, 114); checkroom(AllowMerchantRoom, 1 * 2, "RoomMerchant");
	setdialog(136, 250); checkroom(AllowNobleRoom, 2 * 2, "RoomNoble");
	setdialog(259, 250); checkroom(AllowRoyalRoom, 3 * 2, "RoomRoyal");
	setdialog(692, 90, 80, 20); paint_player_coins();
	setdialog(663, 123); button(pma_butstd, 1, 2, 0, "RoomRest"); fire(inn_rest);
	setdialog(404, 82, 209, 325); paint_description(12, -1, 2);
	auto total = checkout(player_goods, (storefn)room_rent);
	setdialog(138, 387, 125, 20); texta(getnm("Cost"), AlignRightCenter);
	setdialog(285, 387, 80, 20); texta(str("%1i", total), AlignCenterCenter);
	paint_action_panel_na();
}

static void paint_drink() {
	setdialog(140, 117, 40, 20); texta("100", AlignCenterCenter);
	setdialog(195, 111); button(gres("GBTNMED2"), 1, 2, 0, "Rumors");
	setdialog(199, 82, 170, 20); texta(getnm("Drink"), AlignCenterCenter);
	setdialog(137, 82, 48, 20); texta(getnm("Price"), AlignCenterCenter);
	setdialog(134, 23, 238, 28); texta(metrics::h1, getnm("Drinks"), AlignCenterCenter);
	setdialog(400, 23, 238, 28); paint_store_name();
	setdialog(692, 90, 80, 20); paint_player_coins();
	setdialog(403, 82, 232, 20); texta(getnm("Rumors"), AlignCenterCenter);
	setdialog(404, 115, 210, 292); paint_description(12, -1, 2);
	paint_action_panel_player();
}

static void paint_identify() {
	update_items();
	auto total = checkout(player_goods, UserAllowIdentify);
	setdialog(134, 23, 238, 28); texta(metrics::h1, getnm("Identifying"), AlignCenterCenter);
	setdialog(400, 23, 238, 28); paint_store_name();
	setdialog(692, 90, 80, 20); paint_player_coins();
	setdialog(138, 387, 125, 20); texta(getnm("Cost"), AlignRightCenter);
	setdialog(285, 387, 80, 20); texta(str("%1i", total), AlignCenterCenter);
	setdialog(663, 123); button(pma_butstd, 1, 2, 0, "Identify", 3, total); fire(identify_all);
	setdialog(141, 83, 225, 18); texta(getnm("Items"), AlignCenterCenter);
	setdialog(135, 113, 214, 264); paint_identify_items();
	setdialog(404, 82, 209, 325); paint_description(12, -1, 2);
	paint_action_panel_player();
}

static void paint_store() {
	paint_game_dialog("GUISTDRB", get_back_frame());
	paint_right_panel();
	switch(trade_mode) {
	case AllowPeasantRoom: paint_inn(); break;
	case UserPurchaseDrinks: paint_drink(); break;
	case UserAllowIdentify: paint_identify(); break;
	default: paint_buy_sell(); break;
	}
	setdialog(663, 384); button(pma_butstd, 1, 2, KeyEscape, "Done"); fire(buttoncancel);
	paint_game_panel(false, false);
}

void open_store() {
	last_store = bsdata<storei>::elements;
	need_update_items = true;
	room_rent = -1;
	scene(paint_store);
}