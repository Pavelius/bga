#pragma once

#include "nameable.h"
#include "sliceu.h"
#include "variant.h"

enum talkn : unsigned char {
	TalkDialog = 1,
	TalkNoSteal, // "I see you", "You can't steal there"
	TalkWhatDoYouWantToKnown, // What do you want to know?
	TalkISeeYou, // Hello, %ChooseOne. Why you not in the Temple?
	TalkNoSneak,
};

struct talkei {
	unsigned short	index, parent;
	const char*		text;
	variants		elements;
	constexpr explicit operator bool() const { return text != 0; }
	bool			answer() const { return parent != 0; }
	void			clear();
};

struct talki : nameable {
	sliceu<talkei>	elements;
	const talkei*	find(int index) const;
};

talki* find_talk(const talkei* p);

talkei* find_speech(const talki* current_talk, int id);

void read_talk(const char* url);
bool simple_replic(const talkei* p);