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
	switch(v) {
	case UserAllowIdentify: return identify_price;
	case AllowPeasantRoom: return room_prices[0];
	case AllowMerchantRoom: return room_prices[1];
	case AllowNobleRoom: return room_prices[2];
	case AllowRoyalRoom: return room_prices[3];
	default: return 0;
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