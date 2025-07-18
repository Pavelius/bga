#include "bsdata.h"
#include "option.h"

BSDATA(optionfi) = {
	{"DitherAlways"},
	{"GroupDarkvision"},
	{"ShowWeather"},
	{"MaximumHitPointsPerLevel"},
	{"ShowGore"},
	{"PauseCharacterHit"},
	{"PauseCharacterInjured"},
	{"PauseCharacterDeath"},
	{"PauseCharacterAttacked"},
	{"PauseWeaponUnusable"},
	{"PauseEnemyDestroyed"},
	{"PauseEndOfRound"},
	{"PauseEnemySighted"},
	{"PauseSpellCast"},
	{"PauseTrapDetected"},
	{"PauseCharacterCenter"},
};
assert_enum(optionfi, PauseCharacterCenter)

BSDATA(optionvi) = {
	{"GameDifficult", 0, 2, 4},
	{"MouseScrollSpeed", 4, 32, 64},
	{"KeyboardScrollSpeed", 4, 32, 64},
	{"ToolTipsDelay", 200, 2000, 4000},
};
assert_enum(optionvi, ToolTipsDelay)

flagable<1, unsigned> optflags;
optionva optvalues;

optionv optionvi::getindex() const {
	return (optionv)(this - bsdata<optionvi>::elements);
}

static void check_option_values() {
	for(auto i = GameDifficult; i <= ToolTipsDelay; i = (optionv)(i + 1)) {
		auto& ei = bsdata<optionvi>::elements[i];
		if(ei.maximum) {
			if(optvalues[i] < ei.minimal)
				optvalues[i] = ei.minimal;
			else if(optvalues[i] > ei.maximum)
				optvalues[i] = ei.maximum;
		}
	}
}

static void initialize_options() {
	for(auto& e : bsdata<optionvi>())
		optvalues[e.getindex()] = e.start;
	optflags.set(DitherAlways);
	optflags.set(ShowWeather);
}

void read_options() {
	initialize_options();
	check_option_values();
}