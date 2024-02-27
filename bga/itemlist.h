#pragma once

#include "item.h"
#include "scrollable.h"

struct itemlist : scrollable {
	constexpr static size_t maximum_count = 256;
	struct element {
		item*		data;
		int			count;
		bool		ischecked() const { return count >= 1; }
	};
	element			data[maximum_count];
	itemlist() : scrollable() { perscreen = 6; perline = 1; }
	void			add(item* pi);
	void			clear();
	element*		begin() { return data; }
	const element*	end() const { return data + maximum; }
};
struct partyitemlist : itemlist {
	void			update();
};
struct storeitemlist : itemlist {
	void			update();
};