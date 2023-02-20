#include "timer.h"

unsigned long getcputime();

unsigned long current_tick;
unsigned long current_game_tick;

void update_tick() {
	current_tick = getcputime();
}

void update_game_tick() {
	static unsigned long last_tick;
	auto n = current_tick - last_tick;
	if(n < 1000)
		current_game_tick += n;
	last_tick = current_tick;
}