#include "bsreq.h"
#include "stringbuilder.h"
#include "rfiles.h"

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

void initialize_audio() {
	arc_open(bsdata<rfsnd>::source, "art/sound.arc");
	arc_open(bsdata<rfvoc>::source, "art/soundchr.arc");
}