#include "Scylla.h"

#include "NativeWinApi.h"
#include "SystemInformation.h"
#include "ProcessAccessHelp.h"

ConfigurationHolder Scylla::config(L"Scylla.ini");
PluginLoader Scylla::plugins;
ProcessLister Scylla::processLister;

const WCHAR Scylla::DEBUG_LOG_FILENAME[] = L"Scylla_debug.log";
FileLog Scylla::debugLog(DEBUG_LOG_FILENAME);

void Scylla::initAsDll()
{
	ProcessAccessHelp::ownModuleList.clear();

	NativeWinApi::initialize();
	SystemInformation::getSystemInformation();
	ProcessAccessHelp::getProcessModules(GetCurrentProcess(), ProcessAccessHelp::ownModuleList);
}
