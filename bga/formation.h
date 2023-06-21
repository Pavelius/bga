#include "point.h"

#pragma once

enum formation_s : unsigned char {
	NoFormation,
	FormationT, FormationGather, Formation4and2, Formation3by2, FormationProtect,
};
extern formation_s current_formation;

point getformation(point src, point dst, formation_s formation, int index);