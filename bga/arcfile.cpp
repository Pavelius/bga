#include "arcfile.h"
#include "archive.h"
#include "io_stream.h"
#include "slice.h"
#include "stringbuilder.h"

static int arc_compare_name(const void* v1, const void* v2) {
	auto p1 = (arcfile*)v1;
	auto p2 = (arcfile*)v2;
	return szcmp(p1->id, p2->id);
}

template<> void archive::set<arcfile>(arcfile& e) {
	set(e.id);
	set(e.size);
	set(e.offset);
}

static bool arc_serial(archive& a, array& source, const char* url) {
	if(!a.signature("ARC"))
		return false;
	if(a.writemode)
		a.setc<arcfile>(source);
	else {
		// Add to existing records
		unsigned count = 0; a.set(count);
		for(unsigned i = 0; i < count; i++) {
			auto p = (arcfile*)source.add();
			memset(p, 0, source.element_size);
			a.set<arcfile>(*p);
			p->url = url;
		}
	}
	return true;
}

static void arc_sort(array& source) {
	qsort(source.data, source.count, source.element_size, arc_compare_name);
}

void arc_open(array& source, const char* url) {
	io::file file(url, StreamRead);
	if(!file)
		return;
	archive a(file, false);
	auto dup_url = szdup(url);
	if(!arc_serial(a, source, dup_url))
		return;
	arc_sort(source);
}

arcfile* arc_find(array& source, const char* id) {
	arcfile header;
	header.id = id;
	return (arcfile*)bsearch(&header, source.data, source.count, source.element_size, arc_compare_name);
}

void* arc_encoder(const char* url, unsigned size, unsigned offset) {
	io::file file(url, StreamRead);
	if(!file)
		return 0;
	auto data = new unsigned char[size];
	file.seek(offset, SeekSet);
	auto result = file.read(data, size);
	if(result != size) {
		delete[] data;
		return 0;
	}
	return data;
}

void* arcfile::get(fnarcencode encoder) {
	if(!this)
		return 0;
	if(size == -1)
		return 0; // Error occurs lately;
	if(!data) {
		if(!encoder)
			encoder = arc_encoder;
		data = encoder(url, size, offset);
		if(!data)
			size = -1;
	}
	return data;
}

static unsigned get_size(iostream& file) {
	auto push = file.seek(0, SeekCur);
	auto result = file.seek(0, SeekEnd);
	file.seek(push, SeekSet);
	return result;
}

static void read_file(arcfile& e, const char* folder, const char* ext) {
	char temp[260];
	auto pn = szurl(temp, folder, e.id, ext);
	io::file file(pn, StreamRead);
	if(!file)
		return;
	e.size = get_size(file);
	e.data = new unsigned char[e.size];
	file.read(e.data, e.size);
}

static void cashe_files(array& source, const char* folder, const char* ext) {
	io::counter dest;
	archive a(dest, true);
	arc_serial(a, source, 0);
	for(auto& e : source.records<arcfile>()) {
		read_file(e, folder, ext);
		e.offset = dest.count;
		dest.write(0, e.size);
	}
}

bool arc_pack(const char* url, const char* folder, const char* ext) {
	char name[260]; char temp[260]; stringbuilder sb(temp);
	array source(sizeof(arcfile));
	folder = szdup(folder);
	for(io::file::find file(folder); file; file.next()) {
		auto pn = file.name();
		if(pn[0] == '.')
			continue;
		if(szcmpi(szext(pn), ext) != 0)
			continue;
		sb.clear();
		sb.add(szfnamewe(name, pn));
		sb.upper();
		auto p = (arcfile*)source.add();
		memset(p, 0, source.element_size);
		p->id = szdup(temp);
	}
	if(!source)
		return false;
	arc_sort(source);
	cashe_files(source, folder, ext);
	io::file dest(url, StreamWrite);
	if(!dest)
		return false;
	archive a(dest, true);
	if(!arc_serial(a, source, 0))
		return false;
	for(auto& e : source.records<arcfile>())
		a.source.write(e.data, e.size);
	return true;
}