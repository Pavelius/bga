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
	{"ChooseFinish"},
};
assert_enum(commandi, ChooseFinish)

commandn current_step;