#include "ability.h"
#include "bsdata.h"
#include "class.h"
#include "help.h"
#include "skill.h"

BSDATA(helpi) = {
	{"Abilities", bsdata<abilityi>::source},
	{"Classes", bsdata<classi>::source},
	{"Skills", bsdata<skilli>::source},
};
BSDATAF(helpi)