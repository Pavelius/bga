#pragma once

enum damage_s : unsigned char {
	Bludgeon, Slashing, Pierce,
	Acid, Cold, Fire, Sonic,
};
struct damagei {
	const char*	id;
};