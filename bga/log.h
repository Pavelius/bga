#pragma once

namespace log {
typedef void (*fnread)(const char* url);
extern bool allowparse;
void			close();
int				geterrors();
void			error(const char* position, const char* format, ...);
void			errorv(const char* position, const char* format);
const char*		read(const char* url, bool error_if_not_exist = true);
void			readdir(const char* url, const char* filter, fnread proc);
void            setfile(const char* v);
void            seturl(const char* v);
const char*		skipws(const char* p);
const char*		skipwscr(const char* p);
}
