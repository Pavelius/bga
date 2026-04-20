#include "bsreq.h"
#include "draw.h"
#include "stringbuilder.h"
#include "rfiles.h"

BSDATAD(rfpma)
BSDATAD(rfsnd)
BSDATAD(rfvoc)
BSMETA(rfsnd) = {
	BSREQ(id),
	{}};
BSMETA(rfvoc) = {
	BSREQ(id),
	{}};

void* acm_encoder(const char* in_url, unsigned file_size, unsigned file_position, int force_channel);

static void* acm_encoder(const char* url, unsigned file_size, unsigned file_position) {
	return acm_encoder(url, file_size, file_position, 0);
}

void* rfsnd::get() {
	return arcfile::get(acm_encoder);
}

rfsnd* find_sound(const char* id) {
	return (rfsnd*)arc_find(bsdata<rfsnd>::source, id);
}

rfsnd* find_voice(const char* id, int index) {
	char temp[12]; stringbuilder sb(temp);
	sb.add(id);
	auto n = zlen(temp);
	temp[n - 1] = '0' + (index % 10);
	temp[n - 2] = '0' + ((index / 10) % 10);
	return (rfsnd*)arc_find(bsdata<rfvoc>::source, temp);
}

static rfpma* find_image(const char* id) {
	return (rfpma*)arc_find(bsdata<rfpma>::source, id);
}

static void initialize_resources() {
	arc_open(bsdata<rfpma>::source, "art/animations.arc");
	arc_open(bsdata<rfpma>::source, "art/intrface.arc");
	arc_open(bsdata<rfpma>::source, "art/areas.arc");
	arc_open(bsdata<rfpma>::source, "art/characters.arc");
	arc_open(bsdata<rfpma>::source, "art/monsters.arc");
	arc_open(bsdata<rfpma>::source, "art/world.arc");
}

void initialize_audio() {
	arc_open(bsdata<rfsnd>::source, "art/sound.arc");
	arc_open(bsdata<rfvoc>::source, "art/soundchr.arc");
	initialize_resources();
}

sprite* gres(const char* id, const char* folder) {
	return (sprite*)find_image(id)->get();
	//auto p = (residi*)dynamic_headers.findv(id, 0);
	//if(!p) {
	//	p = (residi*)dynamic_headers.add();
	//	p->clear();
	//	p->id = szdup(id);
	//	p->folder = szdup(folder);
	//}
	//return p->get();
}