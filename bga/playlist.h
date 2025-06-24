#pragma once

#include "nameable.h"

enum playlistn : unsigned char {
	PlayDay, PlayNight, PlayCombat,
};
struct playlistni : nameable {
};
struct playlisti {
	const char*	id;
	const char*	folder;
	const char* music[PlayCombat + 1];
};

bool playlist_play(const char* id, const char* folder, playlistn value);
