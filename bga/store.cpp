#include "crt.h"
#include "iteminside.h"
#include "store.h"

BSDATAC(storei, 256);

BSDATA(storefi) = {
	{"UserAllowBuy"},
	{"UserAllowSell"},
	{"UserAllowIdentify"},
	{"UserAllowSteal"},
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

static void store_refresh(variant v) {
	variant parent = last_store;
	if(v.iskind<itemi>()) {
		item it(v.value);
		if(v.counter)
			it.setcount(v.counter);
		add_item(parent, it);
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