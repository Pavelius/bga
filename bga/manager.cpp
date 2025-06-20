#include "io_stream.h"
#include "manager.h"
#include "stringbuilder.h"

void manageri::clear() {
	if(data)
		delete[](char*)data;
	data = 0;
	size = 0;
	error = false;
}

void manager_clear(array& source) {
	auto pe = source.end();
	for(auto pb = source.begin(); pb < pe; pb += source.element_size)
		((manageri*)pb)->clear();
}

static int compare(const void* v1, const void* v2) {
	return szcmp(((nameable*)v1)->id, ((nameable*)v2)->id);
}

void manager_initialize(array& source, const char* folder, const char* filter, bool uppercase) {
	char temp[260]; stringbuilder sb(temp);
	folder = szdup(folder);
	for(io::file::find find(folder); find; find.next()) {
		auto pn = find.name();
		if(!pn || pn[0] == '.')
			continue;
		sb.clear();
		sb.add(pn);
		if(uppercase)
			sb.upper();
		else
			sb.lower();
		if(filter) {
			if(!szpmatch(pn, filter))
				continue;
		}
		szfnamewe(temp, pn);
		sb.reset();
		if(uppercase)
			sb.upper();
		else
			sb.lower();
		auto p = (manageri*)source.findv(temp, 0);
		if(!p) {
			p = (manageri*)source.add();
			memset(p, 0, source.element_size);
		}
		p->clear();
		p->folder = folder;
		p->id = szdup(temp);
	}
	qsort(source.data, source.count, source.element_size, compare);
}

void* manager_get(array& source, const char* id, const char* ext) {
	if(!id || id[0] == 0)
		return 0;
	nameable e; e.id = id;
	auto p = (manageri*)bsearch(&e, source.data, source.count, source.element_size, compare);
	if(!p)
		return 0;
	if(p->data)
		return p->data;
	if(p->error)
		return 0;
	char temp[260]; stringbuilder sb(temp);
	if(p->folder) {
		sb.add(p->folder);
		sb.add("/");
	}
	sb.add(id);
	if(ext) {
		sb.add(".");
		sb.add(ext);
	}
	p->data = loadb(temp, &p->size);
	if(!p->data)
		p->error = true;
	return p->data;
}