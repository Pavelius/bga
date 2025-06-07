#pragma once

#include "item.h"
#include "nameable.h"
#include "variant.h"

enum storefn : unsigned char {
	UserAllowBuy, UserAllowSell, UserAllowIdentify,
	UserAllowDonate, UserPurchaseCures, UserPurchaseDrinks,
	AllowPeasantRoom, AllowMerchantRoom, AllowNobleRoom, AllowRoyalRoom,
};
enum storetn : unsigned char {
	ShopStore, ShopTavern, ShopInn, ShopTemple, ShopContainer,
};
struct storefi : nameable {
};
struct storeti : nameable {
};
struct storei : nameable {
	storetn		type;
	unsigned	flags;
	int			buy_percent, sell_percent;
	int			steal_difficult; // 0 - if steal not availabe. Set default DC 10. Set to 15-20 for hard attemp.
	int			room_prices[4];
	variants	items;
	bool		is(storefn v) const { return (flags & (1 << v)) != 0; }
};
extern storei* last_store;
