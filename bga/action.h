#pragma once

#include "nameable.h"

typedef void(*fnevent)();

enum actionn : unsigned char {
	NoAction,
	ActionDefend, ActionTurn, ActionCast, ActionAttack, ActionUseItem,
	ActionInspiration, ActionTheivery, ActionHide, ActionHead, ActionSearch,
	ActionSpecialAbility, ActionStop, ActionLeft, ActionRight, ActionPlayMusic,
	ActionEntangle, ActionWildernessLore, ActionMeleeAttack, ActionRangeAttack,
	ActionOpenContainer, ActionOpenDoor, ActionPickItems,
};
struct actioni : nameable {
	int			avatar;
	fnevent		proc;
};
