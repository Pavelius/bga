#pragma once

#include "nameable.h"

enum damagen : unsigned char {
	Bludgeon, Slashing, Pierce,
	Acid, Cold, Fire, Sonic,
};
struct damagei : nameable {
};
typedef char damagea[Sonic + 1];