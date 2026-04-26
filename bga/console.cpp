#include "array.h"
#include "console.h"
#include "slice.h"

array console_data(1);

void printclf() {
	if(console_data.count)
		printcnv("\n");
}

void printcnv(const char* format) {
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

void printcnv(const char* format, const char* format_param) {
	if(!format)
		return;
	char temp[4096]; stringbuilder sb(temp);
	sb.addv(format, format_param);
	printclf();
	printcnv(temp);
}

void print(const char* format, ...) {
	XVA_FORMAT(format);
	printcnv(format, format_param);
}