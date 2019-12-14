#include "SystemInformation.h"

OPERATING_SYSTEM SystemInformation::currentOS = UNKNOWN_OS;

bool SystemInformation::getSystemInformation()
{
	OSVERSIONINFOEXW osvi = {0};
	SYSTEM_INFO si = {0};
	def_GetNativeSystemInfo _GetNativeSystemInfo = 0;

	osvi.dwOSVersionInfoSize = sizeof(osvi);
	if (!GetVersionExW((OSVERSIONINFOW*) &osvi))
	{
		return false;
	}

	if ((osvi.dwMajorVersion < 5) || ((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion == 0)))
	{
		return false;
	}

	_GetNativeSystemInfo = (def_GetNativeSystemInfo)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "GetNativeSystemInfo");
	if (_GetNativeSystemInfo)
	{
		_GetNativeSystemInfo(&si);
	}
	else
	{
		GetSystemInfo(&si);
	}

	bool isX64 = si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64;
	bool isX86 = si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL;

	DWORD major = osvi.dwMajorVersion;
	DWORD minor = osvi.dwMinorVersion;

	if(isX64 && major == 5 && minor == 2)
	{
		currentOS = WIN_XP_64;
	}
	else if(isX86 && major == 5 && minor == 1)
	{
		currentOS = WIN_XP_32;
	}
	else if(isX64 && major == 6 && minor == 0)
	{
		currentOS = WIN_VISTA_64;
	}
	else if(isX86 && major == 6 && minor == 0)
	{
		currentOS = WIN_VISTA_32;
	}
	else if(isX64 && major == 6 && minor == 1)
	{
		currentOS = WIN_7_64;
	}
	else if(isX86 && major == 6 && minor == 1)
	{
		currentOS = WIN_7_32;
	}
	else if(isX64 && major == 6 && minor == 2)
	{
		currentOS = WIN_8_64;
	}
	else if(isX86 && major == 6 && minor == 2)
	{
		currentOS = WIN_8_32;
	}
	else if(isX64 && major == 6 && minor == 3)
	{
		currentOS = WIN_8_1_64;
	}
	else if(isX86 && major == 6 && minor == 3)
	{
		currentOS = WIN_8_1_32;
	}
	else if(isX64 && major == 10 && minor == 0)
	{
		currentOS = WIN_10_64;
	}
	else if(isX86 && major == 10 && minor == 0)
	{
		currentOS = WIN_10_32;
	}
	else
	{
		currentOS = UNKNOWN_OS;
	}

	return (currentOS != UNKNOWN_OS);
}
