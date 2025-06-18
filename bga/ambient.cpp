#include "ambient.h"
#include "audio.h"
#include "bsdata.h"
#include "calendar.h"
#include "rand.h"
#include "timer.h"

struct ambientplayer {
	unsigned long	wait_stamp; // Wait until this time. 0 - if none.
	short unsigned	next_sound; // Index of currently played sound.
	bool			playing;
	int	getindex() const { return this - bsdata<ambientplayer>::elements; }
	ambient* getrecord() const { return bsdata<ambient>::elements + getindex(); }
	void clear() { memset(this, 0, sizeof(*this)); }
	void update();
};

static point hearing_center;
BSDATAC(ambientplayer, 32)

void ambient::clear() {
	memset(this, 0, sizeof(*this));
}

int ambient::hearing(point camera) const {
	if(!radius || is(AmbientGlobal))
		return volume;
	else {
		auto i = distance(position, camera);
		if(i >= radius)
			return 0;
		return (100 - i * 100 / radius) * volume / 100;
	}
}

static void finish_playing(void* object, void* callback_object) {
	auto pe = (ambientplayer*)callback_object;
	pe->playing = false;
	auto p = pe->getrecord();
	if(p->delay) {
		auto n = p->delay * 1000;
		if(p->delay_range)
			n += (rand() % (p->delay_range * 2) - p->delay_range) * 1000;
		pe->wait_stamp = current_game_tick + n;
	}
}

void ambientplayer::update() {
	ambient* p = getrecord();
	if(!p)
		return;
	if(playing || !p->sounds)
		return;
	if(wait_stamp) {
		if(current_game_tick < wait_stamp)
			return;
		wait_stamp = 0;
	}
	if(!active_time(p->shedule))
		return;
	short unsigned volume = p->hearing(hearing_center);
	if(!volume)
		return;
	auto n = next_sound;
	if(p->is(AmbientRandom)) {
		next_sound = (unsigned short)(rand() % p->sounds.count);
		n = next_sound;
	} else
		next_sound = (++next_sound) % p->sounds.count;
	play_sound(p->sounds[n].id, volume, finish_playing, this);
	playing = true;
}

void initialize_area_ambients() {
	bsdata<ambientplayer>::source.clear();
	for(auto& e : bsdata<ambient>()) {
		auto p = bsdata<ambientplayer>::add();
		p->clear();
		p->wait_stamp = current_game_tick + xrand(500, 16 * 1000);
	}
}

void update_ambients(point camera) {
	hearing_center = camera;
	for(auto& e : bsdata<ambientplayer>())
		e.update();
}