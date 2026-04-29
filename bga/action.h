#pragma once

#include "nameable.h"

typedef void(*fnevent)();

enum actionn : unsigned char {
	NoAction,
	ActionDefend, ActionTurn, ActionCast, ActionAttack, ActionUseItem,
	ActionInspiration, ActionTheivery, ActionHide, ActionUnkn1, ActionSearch,
	ActionSpecialAbility, ActionStop, ActionLeft, ActionRight, ActionPlayMusic,
	ActionEntangle, ActionWildernessLore, ActionMeleeAttack, ActionRangeAttack,
	ActionOpenItems,
};
struct actioni : nameable {
	int			avatar;
	fnevent		proc;
};
