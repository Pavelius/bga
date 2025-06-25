#include "audio.h"
#include "bsdata.h"
#include "playlist.h"
#include "stringbuilder.h"

BSDATA(playlistni) = {
	{"PlayDay"},
	{"PlayNight"},
	{"PlayCombat"},
};
assert_enum(playlistni, PlayCombat)

static playlisti* find_playlist(const char* id, const char* folder) {
	for(auto& e : bsdata<playlisti>()) {
		if(equal(e.id, id) && equal(e.folder, folder))
			return &e;
	}
	return 0;
}

bool playlist_play(const char* id, const char* folder, playlistn value) {
	auto p = find_playlist(id, folder);
	if(!p)
		return false;
	if(equal(p->music[value], "STOP"))
		play_music(0);
	else
		play_music(p->music[value]);
	return true;
}