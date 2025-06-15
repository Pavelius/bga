#include "action.h"
#include "bsdata.h"

BSDATA(actioni) = {
	{"ActionDefend", 0 * 4},
	{"ActionTurn", 1 * 4},
	{"ActionCast", 2 * 4},
	{"ActionAttack", 3 * 4},
	{"ActionUseItem", 4 * 4},
	{"ActionInspiration", 5 * 4},
	{"ActionTheivery", 6 * 4},
	{"ActionHide", 7 * 4},
	{"ActionUnkn1", 8 * 4},
	{"ActionSearch", 9 * 4},
	{"ActionSpecialAbility", 10 * 4},
	{"ActionStop", 11 * 4},
	{"ActionLeft", 12 * 4},
	{"ActionRight", 13 * 4},
	{"ActionPlayMusic", 14 * 4},
	{"ActionEntangle", 15 * 4},
	{"ActionWildernessLore", 16 * 4},
	{"ActionMeleeAttack", 17 * 4},
	{"ActionRangeAttack", 18 * 4},
};
assert_enum(actioni, ActionRangeAttack)