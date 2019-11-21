#pragma once
#include "Globals.h"

void log(const char file[], int line, const char* format, ...)
{
	//static char tmp_string[4096];
	//static char tmp_string2[4096];
	static va_list  ap;

	// Construct the string from variable arguments
	int old_size = logBuffer.size();
	va_start(ap, format);
	logBuffer.appendfv(format, ap);
	//vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);
	for (int new_size = logBuffer.size(); old_size < new_size; old_size++) {
		if (logBuffer[old_size] == '\n')
			LineOffsets.push_back(old_size);
	}
	ScrollToBottom = true;
	//sprintf_s(tmp_string2, 4096, "\n%s(%d) : %s", file, line, tmp_string);
	//OutputDebugString(tmp_string2);
}