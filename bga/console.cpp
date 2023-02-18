#include "crt.h"

array console_data(1);

void add_console(const char* format) {
	auto i = zlen(format);
	console_data.reserve(console_data.getcount() + i + 1);
	auto m = console_data.getcount();
	if(m) {
		memcpy(console_data.ptr(m - 1), format, i + 1);
		console_data.count += i;
	} else {
		memcpy(console_data.ptr(0), format, i + 1);
		console_data.count += i + 1;
	}
}