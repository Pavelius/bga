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

bool playlist_play(const char* id, playlistn value);
