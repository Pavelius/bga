#pragma once

#include "nameable.h"
#include "flagable.h"

enum optionf : unsigned char {
	DitherAlways, GroupDarkvision, ShowWeather, MaximumHitPointsPerLevel, ShowGore,
};
enum optionv : unsigned char {
	GameDifficult, MouseScrollSpeed, KeyboardScrollSpeed, ToolTipsDelay,
};
struct optionfi : nameable {
};
struct optionvi : nameable {
	int minimal, maximum, start;
	optionv getindex() const;
};
typedef flagable<1, unsigned> optionfa;
typedef int optionva[ToolTipsDelay+1];

extern optionfa optflags;
extern optionva optvalues;

inline int getvalue(optionv v) { return optvalues[v]; }

inline bool getvalue(optionf v) { return optflags.is(v); }

void read_options();