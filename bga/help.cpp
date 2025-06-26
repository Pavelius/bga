#include "ability.h"
#include "bsdata.h"
#include "class.h"
#include "help.h"
#include "feat.h"
#include "skill.h"

static bool filter_ability(const void* object) {
	auto p = ((abilityi*)object);
	return p->player != 0;
}

static bool filter_class(const void* object) {
	auto p = ((classi*)object);
	return p->player != 0;
}

static bool filter_feats(const void* object) {
	auto p = ((feati*)object);
	return p->is(GeneralFeat);
}

BSDATA(helpi) = {
	{"Abilities", bsdata<abilityi>::source, filter_ability},
	{"Classes", bsdata<classi>::source, filter_class},
	{"Feats", bsdata<feati>::source, filter_feats},
	{"Skills", bsdata<skilli>::source},
};
BSDATAF(helpi)