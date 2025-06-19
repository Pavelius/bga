#include "bsdata.h"
#include "command.h"

BSDATA(commandi) = {
	{"ChooseGender"},
	{"ChooseRace"},
	{"ChooseClass"},
	{"ChooseAlignment"},
	{"ChooseAbilities"},
	{"ChooseSkills"},
	{"ChooseAppearance"},
	{"ChooseName"},
};
assert_enum(commandi, ChooseName)

commandn current_step;