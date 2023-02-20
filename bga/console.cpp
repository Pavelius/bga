#include "console.h"
#include "crt.h"

array console_data(1);

void logm_raw(const char* format) {
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

void logm_raw(const char* format, const char* line_feed) {
	if(line_feed && console_data.count)
		logm_raw(line_feed);
	logm_raw(format);
}

void logm_v(const char* format, const char* format_param, const char* line_feed) {
	if(!format)
		return;
	char temp[4096]; stringbuilder sb(temp);
	sb.addv(format, format_param);
	logm_raw(temp, line_feed);
}