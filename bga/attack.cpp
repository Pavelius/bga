#include "attack.h"

void attacki::create(const item& weapon) {
	auto flags = weapon.geti().flags;
	memcpy(static_cast<weaponi*>(this), &weapon.geti().weapon, sizeof(weapon));
	additional.clear();
	burst.clear();
	critical = 0;
	multiplier = 1;
	if(FGT(flags, Balanced))
		critical++;
	if(FGT(flags, UltraBalanced))
		critical += 2;
	if(FGT(flags, Dangerous))
		multiplier++;
	if(FGT(flags, Keen))
		critical += 2;
	if(FGT(flags, Flaming)) {
		additional.type = Fire;
		additional.damage = {1, 6};
	}
	if(FGT(flags, Frost)) {
		additional.type = Cold;
		additional.damage = {1, 6};
	}
	critical = 20 - critical;
}