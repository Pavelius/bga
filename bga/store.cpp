#include "crt.h"
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