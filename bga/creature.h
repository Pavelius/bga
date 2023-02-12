#include "ability.h"
#include "actable.h"
#include "coloration.h"
#include "moveable.h"
#include "portrait.h"
#include "race.h"
#include "wearable.h"

#pragma once

struct creature : moveable, actable, wearable, avatarable, coloration, statable {
	race_s		race;
	statable	basic;
	short		hp, hp_max;
	void		apply(const variants& source);
	void		apply(variant v);
	void		clear();
	void		create(gender_s gender);
	void		update();
	void		update_abilities();
};
extern creature* last_creature;
extern creature* party[6];
