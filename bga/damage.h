#pragma once

#include "nameable.h"

enum damage_s : unsigned char {
	Bludgeon, Slashing, Pierce,
	Acid, Cold, Fire, Sonic,
};
struct damagei : nameable {
};
typedef char damagea[Sonic + 1];