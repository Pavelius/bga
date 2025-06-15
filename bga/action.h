#pragma once

#include "nameable.h"

enum actionn : unsigned char {
	ActionDefend, ActionTurn, ActionCast, ActionAttack, ActionUseItem,
	ActionInspiration, ActionTheivery, ActionHide, ActionUnkn1, ActionSearch,
	ActionSpecialAbility, ActionStop, ActionLeft, ActionRight, ActionPlayMusic,
	ActionEntangle, ActionWildernessLore, ActionMeleeAttack, ActionRangeAttack,
};
struct actioni : nameable {
	int avatar;
};
