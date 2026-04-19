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

static playlisti* find_playlist(const char* id, playlistn type) {
	for(auto& e : bsdata<playlisti>()) {
		if(e.type == type && equal(e.id, id))
			return &e;
	}
	return 0;
}

const char* play_list(const char* id, playlistn value) {
	auto p = find_playlist(id, value);
	if(!p)
		return 0;
	if(equal(p->music, "STOP"))
		return 0;
	else
		return p->music;
}