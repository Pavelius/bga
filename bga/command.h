#pragma once

#include "nameable.h"

enum commandn : unsigned char {
	ChooseGender, ChooseRace, ChooseClass, ChooseAlignment, ChooseAbilities, ChooseSkills, ChooseAppearance, ChooseName,
	ChooseFinish,
};
extern commandn current_step;
struct commandi : nameable {
};
