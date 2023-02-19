#include "ability.h"
#include "actable.h"
#include "collection.h"
#include "coloration.h"
#include "class.h"
#include "drawable.h"
#include "portrait.h"
#include "race.h"
#include "wearable.h"

#pragma once

struct creature : drawable, actable, wearable, classa, avatarable, coloration, statable {
	race_s		race;
	statable	basic;
	unsigned char alignment;
	short		hp, hp_max;
	unsigned	experience;
	void		clear();
	void		create(gender_s gender);
	void		create(race_s race, gender_s gender, class_s classv, unsigned short portrait);
	bool		isclass(skill_s v) const;
	bool		isusable(const item& it) const;
	void		getinfo(stringbuilder& sb) const;
	void		getskillsinfo(stringbuilder& sb) const;
	int			getskillpoints() const;
	void		update();
	void		update_abilities();
};
extern creature* player;
extern creature* party[6];
extern collection<creature> selected_creatures;
