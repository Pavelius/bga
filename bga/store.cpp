#include "bsdata.h"
#include "itemground.h"
#include "store.h"

BSDATAC(storei, 256);

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

short unsigned storei::index() const {
	return this - bsdata<storei>::elements;
}

void storei::add(item& v) {
}

static void store_refresh(variant v) {
	if(v.iskind<itemi>()) {
		item it(v.value);
		if(v.counter)
			it.count = v.counter;
		// add_item(parent, it);
	}
}

static void store_refresh(const variants& source) {
	for(auto v : source)
		store_refresh(v);
}

void initialize_store() {
	auto push_store = last_store;
	for(auto& e : bsdata<storei>()) {
		last_store = &e;
		store_refresh(e.items);
	}
}