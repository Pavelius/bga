#include "bsreq.h"
#include "sndfile.h"

BSDATAD(sndfile)
BSMETA(sndfile) = {
	BSREQ(id),
	{}};

void* acm_encoder(const char* in_url, unsigned file_size, unsigned file_position, int force_channel);

static void* acm_encoder(const char* url, unsigned file_size, unsigned file_position) {
	return acm_encoder(url, file_size, file_position, 0);
}

void* sndfile::get() {
	return arcfile::get(acm_encoder);
}

sndfile* find_sound(const char* id) {
	return (sndfile*)arc_find(bsdata<sndfile>::source, id);
}