#include "alignment.h"
#include "bsdata.h"

BSDATA(alignmenti) = {
	{"LawfulGood"},
	{"NeutralGood"},
	{"ChaoticGood"},
	{"LawfulNeutral"},
	{"TrueNeutral"},
	{"ChaoticNeutral"},
	{"LawfulEvil"},
	{"NeutralEvil"},
	{"ChaoticEvil"},
};
BSDATAF(alignmenti)

alignmentn alignmenti::getindex() const {
	return (alignmentn)(this - bsdata<alignmenti>::elements);
}