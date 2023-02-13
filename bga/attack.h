#include "item.h"

#pragma once

struct attacki : weaponi {
	char		critical;
	char		multiplier;
	weaponi		additional, burst;
	void		create(const item& weapon);
};
