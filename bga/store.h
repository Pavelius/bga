#pragma once

#include "item.h"
#include "nameable.h"
#include "variant.h"

enum storef_s : unsigned char {
	UserAllowBuy, UserAllowSell, UserAllowIdentify, UserAllowSteal,
	UserAllowDonate, UserPurchaseCures, UserPurchaseDrinks,
	AllowPeasantRoom, AllowMerchantRoom, AllowNobleRoom, AllowRoyalRoom,
};
enum storet_s : unsigned char {
	ShopStore, ShopTavern, ShopInn, ShopTemple, ShopContainer,
};
struct storefi : nameable {
};
struct storeti : nameable {
};
struct storei : nameable {
	storet_s		type;
	unsigned		flags;
	int				buy_percent, sell_percent;
	int				chance_steal;
	int				room_prices[4];
	variants		items;
	bool			is(storef_s v) const { return (flags & (1 << v)) != 0; }
};
extern storei* last_store;
