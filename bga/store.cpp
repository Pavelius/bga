#include "bsdata.h"
#include "itemground.h"
#include "game.h"
#include "store.h"

BSDATA(storefi) = {
	{"UserAllowBuy"},
	{"UserAllowSell"},
	{"UserAllowIdentify"},
	{"UserAllowDonate"},
	{"UserPurchaseCures"},
	{"UserPurchaseDrinks"},
	{"AllowPeasantRoom"},
	{"AllowMerchantRoom"},
	{"AllowNobleRoom"},
	{"AllowRoyalRoom"},
};
assert_enum(storefi, AllowRoyalRoom);

BSDATA(storeti) = {
	{"ShopStore"},
	{"ShopTavern"},
	{"ShopInn"},
	{"ShopTemple"},
	{"ShopContainer"},
};
assert_enum(storeti, ShopContainer);

storei* last_store;

int	storei::getcost(storefn v) const {
	int n;
	switch(v) {
	case UserAllowIdentify:
		n = game.get(IdentifyCost);
		if(identify_percent)
			n = n * identify_percent / 100;
		return n;
	case UserAllowBuy:
		n = buy_percent;
		if(!n)
			n = 150;
		n -= game.abilities[Reputation] / 3;
		if(n < 100)
			n = 100;
		return n;
	case UserAllowSell:
		n = sell_percent;
		if(!n)
			n = 40;
		n += game.abilities[Reputation] / 3;
		if(n < 5)
			n = 5;
		else if(n > 95)
			n = 95;
		return n;
	case AllowPeasantRoom:
		return room_prices[0];
	case AllowMerchantRoom:
		return room_prices[1];
	case AllowNobleRoom:
		return room_prices[2];
	case AllowRoyalRoom:
		return room_prices[3];
	default:
		return 0;
	}
}

short unsigned storei::index() const {
	return this - bsdata<storei>::elements;
}

void storei::add(item& v) {
	add_item(0xFFFF, {index(), itemground::Store}, v);
}

static void store_refresh(variant v) {
	if(v.iskind<itemi>()) {
		auto count = v.counter;
		if(!count)
			count = 1;
		item it(v.value, game_rand(count, count * 2));
		last_store->add(it);
	}
}

void store_supply() {
	for(auto v : last_store->items)
		store_refresh(v);
}

void initialize_store() {
	auto push_store = last_store;
	for(auto& e : bsdata<storei>()) {
		last_store = &e;
		store_supply();
		if(game_chance(60))
			store_supply();
		if(game_chance(30))
			store_supply();
	}
}