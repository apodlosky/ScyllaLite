#include "StringConversion.h"
#include <cstdlib>
#include <cstring>

const char* StringConversion::ToASCII(const wchar_t* str, char* buf, size_t buf_count)
{
	size_t length = wcslen(str);
	wcstombs_s(nullptr, buf, buf_count, str, length);
	return buf;
}

const wchar_t* StringConversion::ToUTF16(const char* str, wchar_t* buf, size_t buf_count)
{
	size_t length = strlen(str);
	mbstowcs_s(nullptr, buf, buf_count, str, length);
	return buf;
}
