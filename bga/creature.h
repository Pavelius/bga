#include "actor.h"
#include "actable.h"
#include "collection.h"
#include "portrait.h"

#pragma once

struct creature : actor, avatarable {
	statable		basic;
	unsigned char	alignment;
	short			hp, hp_max;
	unsigned		experience;
	void			clear();
	static void		create(gendern gender);
	static void		create(racen race, gendern gender, classn classv, unsigned short portrait);
	bool			isclass(skill_s v) const;
	bool			isusable(const item& it) const;
	void			paint() const;
	void			update();
	void			update_abilities();
};
extern creature* player;
extern creature* party[6];
extern collection<creature> selected_creatures;

creature* get_creature(const void* object);
