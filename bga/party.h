#pragma once

#include "nameable.h"
#include "resname.h"
#include "feat.h"

enum racen : unsigned char;
enum classn : unsigned char;
enum gendern : unsigned char;
enum alignmentn : unsigned char;

struct partyi : nameable {
	struct character {
		resname name, nickname;
		racen	race;
		gendern	gender;
		classn	type;
		alignmentn alignment;
		char	abilities[6];
		featf	feats;
		char	skills[24];
	};
	character	characters[6];
	const char*	content;
	const char*	description;
	partyi() : content(0), description(0), characters{} {}
	~partyi() { release(); }
	void clear();
	void release();
};
extern partyi* last_party;

void initialize_parties();
void read_party(const char* url);