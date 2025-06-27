#include "draw.h"
#include "io_stream.h"
#include "game.h"
#include "resid.h"
#include "saveheader.h"
#include "slice.h"
#include "vector.h"
#include "view.h"
#include "view_list.h"

using namespace draw;

rowsaveheaderi* last_save_header;

static vector<rowsaveheaderi> files;

void get_save_screenshoot(surface& sm);
void get_player_portrait(surface& sm, int index);

static void update_files() {
	char temp[260];
	files.clear();
	for(io::file::find file("save"); file; file.next()) {
		auto pn = file.name();
		if(pn[0] == '.')
			continue;
		if(!szpmatch(pn, "*.sav"))
			continue;
		auto p = files.add();
		p->clear();
		p->setfile(szfnamewe(temp, pn));
		if(!p->read()) {
			io::file::remove(file.fullname(temp));
			p->clear();
		}
	}
}

static void update_files_save() {
	update_files();
	auto p = files.add();
	p->clear();
}

static int get_file_number(const char* url, const char* mask) {
	char result[260]; stringbuilder sb(result); sb.clear();
	for(io::file::find file(url); file; file.next()) {
		auto pn = file.name();
		if(pn[0] == '.')
			continue;
		if(!szpmatch(pn, mask))
			continue;
		if(!result[0] || szcmp(pn, result) > 0) {
			sb.clear();
			sb.add(pn);
		}
	}
	auto index = 1;
	if(result[0]) {
		char temp[260]; szfnamewe(temp, result);
		auto p = temp + zlen(temp);
		while(p > temp) {
			if(!isnum(p[-1]))
				break;
			p--;
		}
		psnum(p, index);
		index++;
	}
	return index;
}

void saveheaderi::clear() {
	memset(this, 0, sizeof(*this));
}

void saveheaderi::paint() const {
	surface sm; sm.resize(102, 77, 32, false); sm.bits = (unsigned char*)screenshoot;
	blit(*canvas, caret.x, caret.y, sm.width, sm.height, 0, sm, 0, 0);
	sm.bits = 0; // To prevent deallocate bits, when destroy surface.
}

void saveheaderi::paintparty(int index) const {
	surface pm; pm.resize(20, 22, 32, false); pm.bits = (unsigned char*)portraits[index];
	blit(*canvas, caret.x, caret.y, pm.width, pm.height, 0, pm, 0, 0);
	pm.bits = 0; // To prevent deallocate bits, when destroy surface.
}

void rowsaveheaderi::clear() {
	memset(this, 0, sizeof(*this));
}

void rowsaveheaderi::setfile(const char* format, ...) {
	XVA_FORMAT(format)
	stringbuilder sb(file);
	sb.addv(format, format_param);
}

void saveheaderi::create() {
	rounds = game.get(Rounds);
	chapter = game.get(Chapter);
	surface sm; sm.resize(102, 77, 32, false); sm.bits = (unsigned char*)screenshoot;
	get_save_screenshoot(sm);
	surface pm; pm.resize(20, 22, 32, false);
	for(auto i = 0; i < 6; i++) {
		pm.bits = (unsigned char*)portraits[i];
		get_player_portrait(pm, i);
	}
	sm.bits = 0;
	pm.bits = 0;
}

static void row_delete() {
	auto p = (rowsaveheaderi*)hot.object;
	auto m = hot.param;
	if(!confirm("ConfirmDelete"))
		return;
	char temp[260];
	io::file::remove(get_save_url(temp, p->file));
	if(m)
		update_files_save();
	else
		update_files();
}

static void row_save() {
	last_save_header = (rowsaveheaderi*)hot.object;
	if(!last_save_header->file[0])
		last_save_header->setfile("GAM%1.4i", get_file_number("save", "GAM*.sav"));
	if(!confirm_overvrite())
		return;
	last_save_header->serial(true);
	next_scene(open_game);
}

static void row_load() {
	last_save_header = (rowsaveheaderi*)hot.object;
	last_save_header->serial(false);
	next_scene(open_game);
}

static void paint_game_row(void* object, const char* id, fnevent action_proc, int mode) {
	pushrect push;
	pushfore push_fore;
	auto push_header = last_save_header;
	last_save_header = (rowsaveheaderi*)object;
	setdialog(6, 6);  last_save_header->paint();
	setdialog(514, 13); auto push_party = caret;
	for(auto i = 0; i < 6; i++) {
		last_save_header->paintparty(i);
		caret.x += 28;
		if(i == 2) {
			caret.x = push_party.x;
			caret.y += 41;
		}
	}
	setdialog(132, 15, 345, 18);
	texta(NORMAL, *last_save_header ? last_save_header->name : getnm("Empty"), AlignLeft);
	if(*last_save_header) {
		setdialog(132, 40, 279, 18);
		fore = colors::white.mix(colors::black, 192);
		texta(NORMAL, str("%PassedTime\n%RealTime"), AlignLeft);
	}
	fore = push_fore.fore;
	setdialog(604, 11); button(GBTNSTD, 1, 2, 0, id); fire(action_proc, 0, 0, object);
	setdialog(604, 52); button(GBTNSTD, 1, 2, 0, "Delete", 3, last_save_header->file[0] != 0); fire(row_delete, mode, 0, object);
	last_save_header = push_header;
}

static void paint_save_game_row(void* object) {
	paint_game_row(object, "Save", row_save, 1);
}

static void paint_load_game_row(void* object) {
	paint_game_row(object, "Load", row_load, 0);
}

static void paint_game_list(fncommand proc) {
	static int origin;
	const int per_page = 5;
	paint_list(files.data, files.element_size, files.count, origin, per_page, proc, 102, {}, 0, 0, 0, false);
}

static void paint_save_game() {
	paint_game_dialog(GUISRSVB);
	setdialog(243, 22, 311, 28); texta(STONEBIG, getnm("SaveGame"), AlignCenterCenter);
	setdialog(23, 78, 740, 498); paint_game_list(paint_save_game_row);
	setdialog(656, 22); button(GBTNSTD, 1, 2, KeyEscape, "Cancel"); fire(buttoncancel);
}

static void paint_load_game() {
	paint_game_dialog(GUISRSVB);
	setdialog(243, 22, 311, 28); texta(STONEBIG, getnm("LoadGame"), AlignCenterCenter);
	setdialog(23, 78, 740, 498); paint_game_list(paint_load_game_row);
	setdialog(656, 22); button(GBTNSTD, 1, 2, KeyEscape, "Cancel"); fire(buttoncancel);
}

static void paint_confirm_overwrite() {
	paint_dialog(GUISRRQB);
	setdialog(23, 23, 280, 20); texta(NORMAL, getnm("EnterSaveGameName"), AlignCenterCenter);
	setdialog(27, 56, 275, 16); edit(last_save_header->name, sizeof(last_save_header->name) / sizeof(last_save_header->name[0]), AlignLeft);
	setdialog(27, 84, 275, 20); texta(NORMAL, str("%PassedTime"), AlignCenterCenter);
	setdialog(21, 114); button(GBTNSPB1, 1, 2, KeyEscape, "Cancel"); fire(buttoncancel);
	setdialog(149, 114); button(GBTNMED, 1, 2, KeyEnter, "Overwrite"); fire(buttonok);
}

bool confirm_overvrite() {
	caret_index = 0xFFFFFFFF;
	open_dialog(paint_confirm_overwrite, true);
	return getresult() != 0;
}

void open_save_game() {
	update_files_save();
	scene(paint_save_game);
	files.clear();
}

void open_load_game() {
	update_files();
	scene(paint_load_game);
	files.clear();
}