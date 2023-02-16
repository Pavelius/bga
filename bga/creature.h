#include "ability.h"
#include "actable.h"
#include "collection.h"
#include "coloration.h"
#include "class.h"
#include "moveable.h"
#include "portrait.h"
#include "race.h"
#include "wearable.h"

#pragma once

struct creature : moveable, actable, wearable, classa, avatarable, coloration, statable {
	race_s		race;
	statable	basic;
	short		hp, hp_max;
	void		clear();
	void		create(gender_s gender);
	void		create(race_s race, gender_s gender, class_s classv, unsigned short portrait);
	void		update();
	void		update_abilities();
};
extern creature* player;
extern creature* party[6];
extern collection<creature> selected_creatures;
