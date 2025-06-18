#include "ambient.h"
#include "audio.h"
#include "bsdata.h"
#include "calendar.h"
#include "rand.h"
#include "timer.h"

struct ambientplayer {
	unsigned long	wait_stamp; // Wait until this time. 0 - if none.
	short unsigned	next_sound; // Index of currently played sound.
	short			current_channel;
	void clear() { memset(this, 0, sizeof(*this)); current_channel = -1; }
	int	getindex() const { return this - bsdata<ambientplayer>::elements; }
	ambient* getrecord() const { return bsdata<ambient>::elements + getindex(); }
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

static void finish_playing(int channel) {
	auto pe = (ambientplayer*)audio_get_object(channel);
	auto p = pe->getrecord();
	pe->current_channel = -1;
	if(p->delay) {
		auto n = p->delay * 1000;
		if(p->delay_range)
			n += (rand() % (p->delay_range * 2) - p->delay_range) * 1000;
		pe->wait_stamp = current_game_tick + n;
	}
}

static void update_ambient_player(ambientplayer* pe) {
	ambient* p = pe->getrecord();
	if(!p)
		return;
	if(!p->sounds || pe->current_channel != -1)
		return;
	if(pe->wait_stamp) {
		if(current_game_tick < pe->wait_stamp)
			return;
		pe->wait_stamp = 0;
	}
	if(!active_time(p->shedule))
		return;
	short unsigned volume = p->hearing(hearing_center);
	if(!volume)
		return;
	auto n = pe->next_sound;
	if(p->is(AmbientRandom)) {
		pe->next_sound = (unsigned short)(rand() % p->sounds.count);
		n = pe->next_sound;
	} else
		pe->next_sound = (++pe->next_sound) % p->sounds.count;
	play_sound(p->sounds[n].id, volume, finish_playing, pe);
	pe->current_channel = last_channel;
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
		update_ambient_player(&e);
}