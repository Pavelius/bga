#include "stringbuilder.h"

#pragma once

void logm_raw(const char* format);
void logm_raw(const char* format, const char* line_feed);
void logm_v(const char* format, const char* format_param, const char* line_feed);

inline void logm(const char* format, ...) { logm_v(format, xva_start(format), "\n"); }
