#include "collection.h"
#include "crt.h"
#include "portrait.h"

BSDATA(portraiti) = {
	{"NoPortrait"},
	{"DFF", {12, 2, 41, 65}, Female},
	{"DMC", {84, 2, 39, 27}, Male},
	{"DMF", {84, 3, 38, 27}, Male},
	{"DMT", {84, 1, 41, 49}, Male},
	{"EFC", {13, 2, 47, 60}, Female},
	{"EFW", {10, 2, 54, 66}, Female},
	{"EMF", {84, 2, 41, 26}, Male},
	{"EMF2", {12, 0, 39, 41}, Male},
	{"EMT", {12, 2, 66, 37}, Male},
	{"EMW", {13, 6, 51, 49}, Male},
	{"GFW", {85, 0, 40, 58}, Female},
	{"GMT", {12, 0, 66, 14}, Male},
	{"GMW", {84, 2, 41, 46}, Male},
	{"HFF", {84, 2, 100, 98}, Female},
	{"HFT", {84, 0, 39, 66}, Female},
	{"HFW", {12, 0, 49, 66}, Female},
	{"HMB", {87, 0, 41, 49}, Male},
	{"HMC", {12, 0, 49, 53}, Male},
	{"HMF2", {87, 0, 66, 98}, Male},
	{"HMF", {84, 0, 66, 27}, Male},
	{"HMW", {12, 5, 58, 48}, Male},
	{"HaFF", {84, 2, 39, 49}, Female},
	{"HaFT", {12, 0, 39, 14}, Female},
	{"HaMF", {84, 2, 39, 24}, Male},
	{"HaMT", {84, 2, 41, 56}, Male},
	{"HeFB", {12, 2, 39, 56}, Female},
	{"HeFC", {12, 3, 39, 58}, Female},
	{"HeFF", {84, 2, 41, 66}, Female},
	{"HeFW", {84, 0, 41, 40}, Female},
	{"HeMF", {84, 1, 41, 98}, Male},
	{"HeMT", {85, 2, 41, 54}, Male},
	{"HeMW", {12, 0, 49, 41}, Male},
	{"HFW2", {13, 0, 66, 66}, Female},
	{"HMF3", {8, 0, 66, 103}, Male},
	{"EFW2", {83, 0, 60, 60}, Female},
	{"HFW3", {8, 0, 66, 60}, Female},
	{"EFF", {12, 0, 66, 25}, Female},
	{"HMF4", {107, 0, 66, 102}, Male},
	{"EMC", {83, 110, 49, 60}, Male},
	{"HMW2", {12, 0, 22, 41}, Male},
	{"HMW3", {84, 0, 46, 46}, Male},
	{"HMC2", {87, 0, 63, 60}, Male},
	{"HMC3", {12, 79, 94, 91}, Male},
	{"HMF5", {12, 3, 41, 66}, Male},
	{"HMB2", {84, 2, 39, 47}, Male},
	{"DMF2", {12, 3, 66, 58}, Male},
	{"HFT2", {12, 2, 39, 63}, Female},
	{"HFD", {12, 2, 41, 49}, Female},
	{"HFF2", {12, 2, 41, 37}, Female},
	{"HMC4", {12, 1, 39, 56}, Male},
	{"HMF6", {84, 0, 66, 47}, Male},
	{"HMF7", {8, 0, 41, 46}, Male},
	{"HMW4", {13, 0, 107, 66}, Male},
	{"2FBAR1", {84, 3, 41, 40}, Female},
	{"2FDEF1", {0, 79, 66, 41}, Female},
	{"2FELF1", {85, 0, 39, 56}, Female},
	{"2FELF2", {13, 0, 66, 63}, Female},
	{"2FELF3", {8, 4, 51, 53}, Female},
	{"2FGNM1", {107, 79, 50, 51}, Female},
	{"2FHUM1", {13, 0, 49, 66}, Female},
	{"2FHUM2", {85, 0, 41, 40}, Female},
	{"2FHUM3", {84, 2, 46, 45}, Female},
	{"2FORC1", {85, 4, 39, 38}, Female},
	{"2FTIF1", {14, 0, 66, 41}, Female},
	{"2MAAS1", {12, 3, 50, 39}, Male},
	{"2MDEF1", {0, 79, 66, 54}, Male},
	{"2MDEF2", {0, 79, 66, 56}, Male},
	{"2MDWF1", {85, 0, 41, 46}, Male},
	{"2MELF1", {13, 6, 62, 66}, Male},
	{"2MGNM1", {107, 79, 37, 39}, Male},
	{"2MHUM1", {10, 0, 39, 63}, Male},
	{"2MHUM2", {10, 6, 49, 66}, Male},
	{"2MHUM3", {85, 0, 66, 49}, Male},
	{"2MHUM4", {85, 0, 41, 66}, Male},
	{"2MHUM5", {10, 2, 65, 39}, Male},
	{"2MORC1", {112, 0, 66, 41}, Male},
	{"2MPAL1", {84, 2, 41, 66}, Male},
	{"2FHUM4", {85, 0, 50, 39}, Female},
	{"2MBAR1", {85, 0, 41, 39}, Male},
	{"2MDWF2", {85, 2, 39, 56}, Male},
	{"2MHAF1", {85, 2, 66, 38}, Male},
	{"2MHUM6", {85, 2, 41, 39}, Male},
	{"2MHUM7", {8, 0, 39, 66}, Male},
};
BSDATAF(portraiti)

static bool filter_gender(const void* object, int param) {
	return ((portraiti*)object)->gender == (gender_s)param;
}

short unsigned random_portrait(gender_s gender) {
	collection<portraiti> source;
	source.select();
	source.match(filter_gender, gender, true);
	if(!source)
		return 0;
	return source.random() - bsdata<portraiti>::elements;
}