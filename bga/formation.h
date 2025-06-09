#include "point.h"

#pragma once

enum formationn : unsigned char {
	NoFormation,
	FormationT, FormationGather, Formation4and2, Formation3by2, FormationProtect,
};
extern formationn current_formation;

point get_formation(point src, point dst, formationn formation, int index);