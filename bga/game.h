#pragma once

#include "nameable.h"
#include "point.h"
#include "slice.h"

enum actionn : unsigned char;

enum gamen : unsigned char {
	Reputation, Progress, Chapter, Blessing, Rounds,
	IdentifyCost,
};
struct gameni : nameable {
};
struct gamei {
	int		abilities[IdentifyCost + 1];
	void	clear();
	int		get(gamen i) const { return abilities[i]; }
	void	set(gamen i, int v) { abilities[i] = v; }
};
extern gamei game;

extern int last_number;

const char* get_save_url(char* result, const char* id);

int getminute();
int game_rand(int v1, int v2);

void debugmsg(const char* format, ...);
void enter(const char* location);
void enter_from_wmap(const char* area);
void initialize_story();
bool is_saved_game(const char* url);
void game_appear(bool need_next_scene = true);
void game_auto_save();
bool game_chance(int v);
void game_disappear();
void game_quick_load();
void game_quick_save();
void party_action(point target_position, actionn action, short unsigned param);
void party_move(point v);
void setcamera(point v);
void setparty(point v, unsigned char orientation = 0xFF);
void setpartyall(point v);