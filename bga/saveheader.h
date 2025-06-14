#pragma once

#include "color.h"
#include "namestr.h"
#include "io_stream.h"

struct saveheaderi : namestr {
	color screenshoot[102][77];
	color party[6][20][22];
	unsigned rounds, chapter;
	io::file::datei change;
	void clear();
	void create();
	void paint() const;
	void paintparty(int index) const;
	bool read(const char* id);
};
struct rowsaveheaderi : saveheaderi {
	char file[12];
	void clear();
	bool serial(bool writemode);
	void setfile(const char* format, ...);
};
extern rowsaveheaderi* last_save_header;