#include "ability.h"
#include "actable.h"
#include "coloration.h"
#include "moveable.h"
#include "portrait.h"
#include "wearable.h"

#pragma once

struct creature : moveable, actable, wearable, avatarable, coloration, statable {
	statable	basic;
	short		hp, hp_max;
	void		clear();
	void		create(gender_s gender);
	void		update();
	void		update_abilities();
};
extern creature* last_creature;
