#include "Scylla.h"

#include "NativeWinApi.h"
#include "SystemInformation.h"
#include "ProcessAccessHelp.h"

const WCHAR Scylla::DEBUG_LOG_FILENAME[] = L"Scylla_debug.log";
FileLog Scylla::debugLog(DEBUG_LOG_FILENAME);

void Scylla::initAsDll()
{
	ProcessAccessHelp::ownModuleList.clear();

	NativeWinApi::initialize();
	SystemInformation::getSystemInformation();
	ProcessAccessHelp::getProcessModules(GetCurrentProcess(), ProcessAccessHelp::ownModuleList);
}
