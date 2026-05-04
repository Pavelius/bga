#include "array.h"
#include "io_stream.h"
#include "nameable.h"
#include "stringbuilder.h"

void initialize_data(array& source, const char* folder, const char* pattern) {
	char temp[64], url[260]; stringbuilder sb(url);
	sb.add(folder, current_locale);
	for(io::file::find e(url); e; e.next()) {
		if(e.name()[0] == '.')
			continue;
		if(!szpmatch(e.name(), pattern))
			continue;
		auto p = (nameable*)source.add();
		memset(p, 0, source.element_size);
		szfnamewe(temp, e.name());
		szupper(temp);
		p->id = szdup(temp);
	}
}