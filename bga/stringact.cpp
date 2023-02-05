#include "crt.h"
#include "stringact.h"

namespace {
struct gender_change_string {
	const char*	female;
	const char*	male;
	const char*	multiply;
};
static gender_change_string player_gender[] = {
	{"госпожа", "господин", "господа"},
	{"женщина", "мужчина", "господа"},
	{"стерва", "ублюдок", "ублюдки"},
	{"миледи", "милорд", "милорды"},
	{"леди", "лорд", "лорды"},
	{"такая", "такой", "такие"},
	{"она", "он", "они"},
	{"шла", "шел", "шли"},
	{"нее", "него", "них"},
	{"ась", "ся", "ись"},
	{"ая", "ый", "ые"},
	{"ей", "ему", "им"},
	{"ла", "", "ли"},
	{"ее", "его", "их"},
	{"а", "", "и"},
};
}

void stringact::addidentifier(const char* identifier) {
	if(name && (equal(identifier, "герой") || equal(identifier, "name")))
		add(name);
	else if(name && strcmp(identifier, "героя") == 0)
		addof(name);
	else if(name && strcmp(identifier, "герою") == 0)
		addto(name);
	else {
		for(auto& e : player_gender) {
			if(strcmp(e.female, identifier) != 0)
				continue;
			if(gender == NoGender)
				add(e.multiply);
			else if(gender == Female)
				add(e.female);
			else
				add(e.male);
			return;
		}
		stringbuilder::addidentifier(identifier);
	}
}