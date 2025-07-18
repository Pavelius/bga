#pragma once

#include "nameable.h"
#include "point.h"
#include "slice.h"

enum gamen : unsigned char {
	Reputation, Progress, Chapter, Rounds,
	IdentifyCost,
};
struct gameni : nameable {
};
struct gamei {
	int abilities[IdentifyCost + 1];
	int get(gamen i) const { return abilities[i]; }
	void set(gamen i, int v) { abilities[i] = v; }
};
extern gamei game;

const char* get_save_url(char* result, const char* id);

void create_game();
void enter(const char* id, const char* location);
void initialize_story();
bool is_saved_game(const char* url);
void game_auto_save();
void game_quick_load();
void game_quick_save();
void party_action(void* object, point target_position, fnevent apply);
void party_move(point v);
void setcamera(point v);
void setparty(point v, unsigned char orientation = 0xFF);
void setpartyall(point v);
