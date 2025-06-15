#include "bsdata.h"
#include "option.h"

BSDATA(optionfi) = {
	{"DitherAlways"},
	{"GroupDarkvision"},
	{"ShowWeather"},
	{"MaximumHitPointsPerLevel"},
	{"ShowGore"},
};
assert_enum(optionfi, ShowGore)

BSDATA(optionvi) = {
	{"GameDifficult", 0, 4, 2},
	{"MouseScrollSpeed", 4, 64, 32},
	{"KeyboardScrollSpeed", 4, 64, 32},
	{"ToolTipsDelay", 1000, 4000, 2000},
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