#pragma once

#include "nameable.h"
#include "flagable.h"

enum optionf : unsigned char {
	DitherAlways, GroupDarkvision, ShowWeather, MaximumHitPointsPerLevel, ShowGore,
	PauseCharacterHit, PauseCharacterInjured, PauseCharacterDeath, PauseCharacterAttacked,
	PauseWeaponUnusable, PauseEnemyDestroyed, PauseEndOfRound, PauseEnemySighted, PauseSpellCast,
	PauseTrapDetected, PauseCharacterCenter,
};
enum optionv : unsigned char {
	GameDifficult, MouseScrollSpeed, KeyboardScrollSpeed, ToolTipsDelay,
};
struct optionfi : nameable {
};
struct optionvi : nameable {
	int minimal, start, maximum;
	optionv getindex() const;
};
typedef flagable<1, unsigned> optionfa;
typedef int optionva[ToolTipsDelay+1];

extern optionfa optflags;
extern optionva optvalues;

inline int getvalue(optionv v) { return optvalues[v]; }

inline bool getvalue(optionf v) { return optflags.is(v); }

void read_options();