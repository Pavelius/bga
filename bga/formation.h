#include "point.h"

#pragma once

enum formation_s : unsigned char {
	NoFormation,
	FormationT, FormationGather, Formation4and2, Formation3by2, FormationProtect,
};

point getformation(point src, point dst, formation_s formation, int index);