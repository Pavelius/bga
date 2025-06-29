#pragma once

#include "nameable.h"

enum playlistn : unsigned char {
	PlayDay, PlayNight, PlayCombat,
};
struct playlistni : nameable {
};
struct playlisti {
	playlistn type;
	const char*	id;
	const char* music;
};

bool play_list(const char* id, playlistn value = PlayDay);
