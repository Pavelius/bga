#pragma once

struct array;

typedef void*(*fnarcencode)(const char* url, unsigned size, unsigned position);

struct arcfile {
	const char*	id; // File name
	const char*	url; // Archive file source
	unsigned	size; // Size of file
	unsigned	offset; // Offset of file from start of archive
	void*		data; // This is cashed value. 0 - if not cashed
	void*		get(fnarcencode encoder);
};

int arc_compare_name(const void* v1, const void* v2);

void arc_open(array& source, const char* url);
bool arc_pack(const char* url, const char* folder, const char* ext);

arcfile* arc_find(array& source, const char* id);

void* arc_encoder(const char* url, unsigned size, unsigned offset);