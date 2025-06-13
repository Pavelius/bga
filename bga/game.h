#pragma once

#include "nameable.h"
#include "point.h"

enum gamen : unsigned char {
	Reputation, Progress,
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

void create_game();
void enter(const char* id, const char* location);
void gamesave(const char* name);
void moveparty(point v);
void setcamera(point v);
void setparty(point v);
void setpartyall(point v);
