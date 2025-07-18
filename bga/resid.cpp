#include "bsdata.h"
#include "io_stream.h"
#include "resid.h"
#include "stringbuilder.h"

static const char* font = "art/fonts";
static const char* root = "art/pma";
static const char* chra = "art/characters";
static const char* mons = "art/monsters";
static const char* proj = "art/projectiles";
static const char* wrld = "art/worldmap";

static array dynamic_headers(sizeof(residi));

BSDATA(residi) = {
	{"NONE"},
	{"STONEBIG", font}, {"REALMS", font}, {"NORMAL", font}, {"TOOLFONT", font}, {"NUMBER", font}, {"TEST", font},
	{"CURSORS", root}, {"CURSARW", root}, {"COLGRAD", root}, {"COLOR", root}, {"TOOLTIP", root},
	{"GACTN", root},
	{"GBTNBFRM", root}, {"GBTNJBTN", root}, {"GBTNLRG", root}, {"GBTNMED", root}, {"GBTNMED2", root}, {"GBTNMINS", root}, {"GBTNPLUS", root},
	{"GBTNOPT1", root}, {"GBTNOPT2", root}, {"GBTNOPT3", root}, {"GUISLDR", root},
	{"GBTNSPB1", root}, {"GBTNSPB2", root}, {"GBTNSPB3", root},
	{"GBTNPOR", root}, {"GUICONT", root}, {"GUIMAP", root}, {"GUIMAPWC", root}, {"GUIHITPT", root}, {"GUIREC", root}, {"GBTNRECB", root}, {"GBTNSCRL", root}, {"GBTNSTD", root},
	{"GCOMM", root}, {"GCOMMSB", root}, {"GCOMMBTN", root}, {"FLAG1", root},
	{"INVBUT2", root}, {"INVBUT3", root}, {"ITEMS", root}, {"GROUND", root}, {"CONTAINER", root}, {"SCROLLS", root}, {"FORM", root},
	{"CDMB1", chra}, {"CDMB2", chra}, {"CDMB3", chra}, {"CDMC4", chra}, {"CDMF4", chra}, {"CDMT1", chra}, {"CDMW1", chra}, {"CDMW2", chra}, {"CDMW3", chra}, {"CDMW4", chra},
	{"CEFB1", chra}, {"CEFB2", chra}, {"CEFB3", chra}, {"CEFC4", chra}, {"CEFF4", chra}, {"CEFT1", chra}, {"CEFW1", chra}, {"CEFW2", chra}, {"CEFW3", chra}, {"CEFW4", chra},
	{"CEMB1", chra}, {"CEMB2", chra}, {"CEMB3", chra}, {"CEMC4", chra}, {"CEMF4", chra}, {"CEMT1", chra}, {"CEMW1", chra}, {"CEMW2", chra}, {"CEMW3", chra}, {"CEMW4", chra},
	{"CHFB1", chra}, {"CHFB2", chra}, {"CHFB3", chra}, {"CHFC4", chra}, {"CHFF4", chra}, {"CHFT1", chra}, {"CHFW1", chra}, {"CHFW2", chra}, {"CHFW3", chra}, {"CHFW4", chra}, {"CHFM1", chra},
	{"CHMB1", chra}, {"CHMB2", chra}, {"CHMB3", chra}, {"CHMC4", chra}, {"CHMF4", chra}, {"CHMM1", chra}, {"CHMW1", chra}, {"CHMW2", chra}, {"CHMW3", chra}, {"CHMW4", chra}, {"CHMT2", chra},
	{"CIFB1", chra}, {"CIFB2", chra}, {"CIFB3", chra}, {"CIFC4", chra}, {"CIFF4", chra}, {"CIFT1", chra},
	{"CIMB1", chra}, {"CIMB2", chra}, {"CIMB3", chra}, {"CIMC4", chra}, {"CIMF4", chra}, {"CIMT1", chra},
	{"GUIACG", root}, {"GUIBTACT", root}, {"GUIBTBUT", root},
	{"GUICGB", root}, {"GUICHP", root}, {"GUIHELP", root}, {"GUIINV", root},
	{"GIITMH08", root}, {"GUIHSB", root}, {"GUICARBB", root}, {"GUISTDRB", root},
	{"GUIPFC", root}, {"GUIRSPOR", root}, {"GUISPL", root}, {"GUICNAME", root}, {"GUIERR", root},
	{"GUIRLVL", root}, {"GMPMCHRB", root}, {"GUISTMSC", root}, {"GCGPARTY", root}, {"GBTNLRG2", root},
	{"GUIJRNL", root}, {"GOPT", root}, {"GUIMOVB", root}, {"STONEOPT", root}, {"GUISRSVB", root}, {"GUISTBBC", root}, {"GUISRRQB", root}, {"GUWBTP", root},
	{"CGEAR", root}, {"ROOMS", root},
	{"LOAD", root}, {"LOCATER", root},
	{"MAPICONS", wrld},
	{"PORTL", root}, {"PORTS", root},
	{"SPELLS", root}, {"SPLBUT", root}, {"START", root}, {"STON", root}, {"STONSLOT", root},
	{"WMAP1", wrld}, {"WMAP2", wrld}, {"WMAP3", wrld},
	{"WQSAX", chra}, {"WQNAX", chra}, {"WQMAX", chra}, {"WQLAX", chra},
	{"WQSBW", chra}, {"WQNBW", chra}, {"WQMBW", chra}, {"WQLBW", chra},
	{"WQSCB", chra}, {"WQNCB", chra}, {"WQMCB", chra}, {"WQLCB", chra},
	{"WQSD1", chra}, {"WQND1", chra}, {"WQMD1", chra}, {"WQLD1", chra},
	{"WQSD2", chra}, {"WQND2", chra}, {"WQMD2", chra}, {"WQLD2", chra},
	{"WQSD3", chra}, {"WQND3", chra}, {"WQMD3", chra}, {"WQLD3", chra},
	{"WQSD4", chra}, {"WQND4", chra}, {"WQMD4", chra}, {"WQLD4", chra},
	{"WQSDD", chra}, {"WQNDD", chra}, {"WQMDD", chra}, {"WQLDD", chra},
	{"WQSFL", chra}, {"WQNFL", chra}, {"WQMFL", chra}, {"WQLFL", chra},
	{"WQSH0", chra}, {"WQNH0", chra}, {"WQMH0", chra}, {"WQLH0", chra},
	{"WQSH1", chra}, {"WQNH1", chra}, {"WQMH1", chra}, {"WQLH1", chra},
	{"WQSH2", chra}, {"WQNH2", chra}, {"WQMH2", chra}, {"WQLH2", chra},
	{"WQSH3", chra}, {"WQNH3", chra}, {"WQMH3", chra}, {"WQLH3", chra},
	{"WQSH4", chra}, {"WQNH4", chra}, {"WQMH4", chra}, {"WQLH4", chra},
	{"WQSH5", chra}, {"WQNH5", chra}, {"WQMH5", chra}, {"WQLH5", chra},
	{"WQSHB", chra}, {"WQNHB", chra}, {"WQMHB", chra}, {"WQLHB", chra},
	{"WQSMC", chra}, {"WQNMC", chra}, {"WQMMC", chra}, {"WQLMC", chra},
	{"WQSQS", chra}, {"WQNQS", chra}, {"WQMQS", chra}, {"WQLQS", chra},
	{"WQSS1", chra}, {"WQNS1", chra}, {"WQMS1", chra}, {"WQLS1", chra},
	{"WQSS2", chra}, {"WQNS2", chra}, {"WQMS2", chra}, {"WQLS2", chra},
	{"WQSS3", chra}, {"WQNS3", chra}, {"WQMS3", chra}, {"WQLS3", chra},
	{"WQSSC", chra}, {"WQNSC", chra}, {"WQMSC", chra}, {"WQLSC", chra},
	{"WQSSL", chra}, {"WQNSL", chra}, {"WQMSL", chra}, {"WQLSL", chra},
	{"WQSSP", chra}, {"WQNSP", chra}, {"WQMSP", chra}, {"WQLSP", chra},
	{"WQSSS", chra}, {"WQNSS", chra}, {"WQMSS", chra}, {"WQLSS", chra},
	{"WQSWH", chra}, {"WQNWH", chra}, {"WQMWH", chra}, {"WQLWH", chra},
	{"ARARROW", proj},
	{"MGO1", mons}, {"MSKA", mons}, {"MSKAA", mons},
};
assert_enum(residi, MSKAA)

void residi::clear() {
	memset(this, 0, sizeof(*this));
}

sprite* residi::get() {
	if(data)
		return data;
	if(!data && !error) {
		char temp[260]; szurl(temp, folder, id, "pma");
		data = (sprite*)loadb(temp);
		error = (data != 0);
	}
	return data;
}

sprite* gres(resn i) {
	if(i == NONE)
		return 0;
	return bsdata<residi>::elements[i].get();
}

sprite* gres(const char* id, const char* folder) {
	auto p = (residi*)dynamic_headers.findv(id, 0);
	if(!p) {
		p = (residi*)dynamic_headers.add();
		p->clear();
		p->id = szdup(id);
		p->folder = szdup(folder);
	}
	return p->get();
}