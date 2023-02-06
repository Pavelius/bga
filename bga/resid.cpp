#include "crt.h"
#include "resid.h"

using namespace res;

BSDATA(residi) = {
	{"NONE"},
	{"CURSORS"}, {"CURSARW"}, {"COLGRAD"}, {"COLOR"},
	{"GACTN"},
	{"GBTNBFRM"}, {"GBTNJBTN"}, {"GBTNLRG"}, {"GBTNMED"}, {"GBTNMED2"}, {"GBTNMINS"}, {"GBTNPLUS"}, {"GBTNOPT1"}, {"GBTNOPT2"},
	{"GBTNSPB1"}, {"GBTNSPB2"}, {"GBTNSPB3"},
	{"GBTNPOR"}, {"GUICONT"}, {"GUIMAP"}, {"GUIMAPWC"}, {"GUIREC"}, {"GBTNRECB"}, {"GBTNSCRL"}, {"GBTNSTD"},
	{"GCOMM"}, {"GCOMMBTN"}, {"FLAG1"},
	{"INVBUT2"}, {"INVBUT3"}, {"ITEMS"}, {"GROUND"}, {"CONTAINER"}, {"SCROLLS"}, {"FORM"},
};
assert_enum(residi, res::FORM)

static const char* getfolder(token i) {
	if((i >= WQSAX && i <= WQLWH)
		|| (i >= CDMB1 && i <= CIMT1))
		return "art/characters";
	return "art/pma";
}

const sprite* gres(res::token i) {
	if(i == NONE)
		return 0;
	auto p = bsdata<residi>::elements + i;
	if(!p->data && !p->error) {
		char temp[260]; szurl(temp, getfolder(i), p->id, "pma");
		p->data = (sprite*)loadb(temp);
		p->error = (p->data != 0);
	}
	return p->data;
}