#include "ambient.h"
#include "audio.h"
#include "bsdata.h"
#include "calendar.h"
#include "rand.h"
#include "sndfile.h"
#include "timer.h"

struct ambientplayer {
	int				delay; // Wait until this time. 0 - if none.
	short unsigned	next_sound; // Index of currently played sound.
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

void ambientplayer::update() {
	ambient* p = getrecord();
	if(!p)
		return;
	if(!p->sounds)
		return;
	if(!delay)
		delay = xrand(2 * 1000, 10 * 1000);
	if(delay > 0) {
		delay -= current_tick_delta;
		return;
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
	auto ps = find_sound(p->sounds[n].id);
	if(!ps)
		return;
	if(audio_played(ps->get()))
		return;
	delay = audio_lenght(ps->get());
	delay += p->delay * 1000;
	if(p->delay_range)
		delay += (rand() % p->delay) * 1000;
	play_sound(ps);
}

void initialize_area_ambients() {
	bsdata<ambientplayer>::source.clear();
	for(auto& e : bsdata<ambient>()) {
		auto p = bsdata<ambientplayer>::add();
		p->clear();
	}
}

void update_ambients(point camera) {
	hearing_center = camera;
	for(auto& e : bsdata<ambientplayer>())
		e.update();
}