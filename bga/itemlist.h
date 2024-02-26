#pragma once

#include "item.h"
#include "scrolltext.h"

struct itemlist : scrollable {
	constexpr static size_t maximum_count = 256;
	struct element {
		item*		data;
		int			count;
		bool		ischecked() const { return count >= 1; }
	};
	bool			is_updated;
	element			data[maximum_count];
	itemlist() : is_updated(false), scrollable() { perscreen = 6; perline = 1; }
	void			add(item* pi);
	void			clear();
	element*		begin() { return data; }
	const element*	end() const { return data + maximum; }
	void			invalidate();
	virtual void	update() = 0;
};
struct partyitemlist : itemlist {
	void			update();
};
struct storeitemlist : itemlist {
	void			update();
};