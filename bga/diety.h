#pragma once

#include "nameable.h"
#include "skill.h"

struct spelli;

enum dietyn : unsigned char {
	NoDiety,
};
struct dietyfi : nameable {
};
struct dietyi : nameable {
	spelli*	powers[2];
	spelli*	spells[10];
	char	powers_use_per_day[2];
	skillf	skills;
};
